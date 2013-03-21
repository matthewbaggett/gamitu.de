<?php 
use tcore\model;

class user extends model{
	protected $_key = 'uid';
	protected $uid;
	protected $username;
	protected $password;
	protected $email;
	protected $access_token;
	protected $access_token_expiry;
	protected $_access_token_expiry_type = self::TYPE_DATE;
	protected $google_id;
	protected $firstname;
	protected $surname;
	protected $link;
	protected $_link_type = self::TYPE_TEXT;
	protected $face;
	protected $_face_type = self::TYPE_TEXT;
	protected $gender;
	protected $birthday;
	protected $_birthday_type = self::TYPE_DATE;
	protected $locale;
    
    const MAX_LATITUDE_LOCATIONS_PER_REQUEST = 1000;
	
	public function login(){
		$_SESSION['user'] = serialize($this);
	}
	
	public function logout(){
		unset($_SESSION['user']);
	}
	
	static public function get_current_user(){
		if(self::is_loggedin()){
			return unserialize($_SESSION['user']);
		}else{
			return FALSE;
		}
	}
	
	static public function is_loggedin(){
		if(isset($_SESSION['user'])){
			if(unserialize($_SESSION['user']) instanceof user){
				return TRUE;
			}
		}
		return FALSE;
	}
	
	static public function attempt_login($user, $password){
		$oUser = self::factory()
					->search()
						->where('username', $user)
						->execOne();
		if($oUser){
			if(crypt($password, $oUser->get_password()) == $oUser->get_password()){
				$oUser->login();
				return TRUE;
			}else{
				return FALSE;
			}
		}
		return FALSE;
	}
	
	static public function attempt_login_with_google($google_id){
		$oUser = self::factory()
			->search()
				->where('google_id', $google_id)
				->execOne();
		if($oUser){
			$oUser->login();
			return $oUser;
		}
		return FALSE;
	}
	
	public function update_latitude_locations(){
		$newest_point_on_record = point::factory()
			->search()
				->where('uid', $this->get_uid())
				->limit(1)
				->order('timestamp_ms','DESC')
				->execOne();
		
		header("Content-Type: text/plain");
		
		echo "Requesting {$this->username}\n";
		// Create the Google Latitude access object
		echo " > Get Google Latitude object\n";
		$latitude = eden('google')->latitude(GOOGLE_API_KEY, $this->get_access_token());
			
		// Create the Google Latitude History access object
		echo " > Get Google Latitude History object\n";
		$history = $latitude->history();
		$history->setGranularity('best');
		$history->setMaxResults(user::MAX_LATITUDE_LOCATIONS_PER_REQUEST);
			
		// Request a list of places from Google Latitude History
		echo " > Requesting places";
		$places = $history->fetch();
		echo " (found ".count($places).")\n";
		
		// Process the places
		return $this->_process_places($places);
	}

	static public function get_all_users(){
		$users = user::factory()
					->search()
						->where('access_token','',"!=")
						->limit(50)
						->exec();
		return $users;
		
	}
	
	public function get_last_two_weeks_of_latitude_locations(){

		// Create the Google Latitude access object
		$latitude = eden('google')->latitude(GOOGLE_API_KEY, $this->get_access_token());
		
		// Create the Google Latitude History access object
		$history = $latitude->history();
		$history->setGranularity('best');
		$history->setMaxResults(user::MAX_LATITUDE_LOCATIONS_PER_REQUEST);
		
		// Work out our starting timestamp
		$time_ms_two_weeks_ago = strtotime("two weeks ago") * 1000;
		
		$current_oldest = $time_ms_two_weeks_ago;
		$places_pool = array();
		while($current_oldest < strtotime("yesterday")){
			
			$history->setMinTime($current_oldest);
			$history->setMaxResults(1000);
				
			// Request a list of places from Google Latitude History
			$places = $history->fetch();
			
			$places_pool = array_merge($places_pool, $places);
			
		}
		// Process the places
		$this->_process_places($places_pool);
	}
	
	protected function _process_places($places){
		$added = 0;
		$ignored = 0;
		foreach($places as $place){
			$oNewPoint = new point();
			$oNewPoint->set_timestamp_ms($place->getTimestampMs());
			$oNewPoint->set_latitude($place->getLatitude());
			$oNewPoint->set_longitude($place->getLongitude());
			$oNewPoint->set_uid($this->get_uid());
			$human_time = date("Y-m-d H:i:s", $oNewPoint->get_timestamp_ms());
			
			if(!$oNewPoint->already_exists()){
                $oNewPoint->save();
				echo "Adding point {$oNewPoint->get_uid()}:{$oNewPoint->get_latitude()}/{$oNewPoint->get_longitude()}@{$human_time} ";
				echo "Saved as {$oNewPoint->get_pid()}\n";
				$added++;
			}else{
				$ignored++;
			}
			
		}
		echo "Added {$added}, ignored {$ignored}\n\n";
		return array($added, $ignored);
	}
	
	public function get_last_location(){
		$location = point::factory()
						->search()
							->where('uid', $this->get_uid())
							->order('timestamp_ms','DESC')
							->execOne();
		return $location;
	}
}