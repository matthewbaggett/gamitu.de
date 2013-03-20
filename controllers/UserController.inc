<?php 

class UserController extends BaseController{
	public function RegisterGoogleAction(){
		$scopes = array(
				'userinfo',
				'latitude_all_best', 
				'latitude_all_city', 
				'latitude_current_best', 
				'latitude_current_city'
		);
		$auth = eden('google')
			->auth(
				GOOGLE_CLIENT_ID, 
				GOOGLE_CLIENT_SECRET, 
				GOOGLE_LOGIN_REDIRECT, 
				GOOGLE_API_KEY);
		
		
		//if no code and no session
		if(!isset($_GET['code']) && !isset($_SESSION['token'])) {
			//redirect to login
			$login = $auth->getLoginUrl($scopes);
			header('Location: '.$login);
			exit;
		}
		
		//Code is returned back from google
		
		if(isset($_GET['code'])) {
			//save it to session
			$access = $auth->forOffline()->getAccess($_GET['code']);
			
			$userinfo = eden('google')->userinfo($access['access_token']);
			$profile = $userinfo->me();
			
			//First try to log them in with these credentials:
			$log_in_success = user::factory()->attempt_login_with_google($profile['id']);
			if($log_in_success instanceof user){
				$log_in_success->set_access_token($access['access_token'])->save();
				header("Location: /user/edit");
			}else{
				$token_expire = time() + $access['expires_in'];
				$oUser = user::factory()
					->set_access_token($access['access_token'])
					->set_access_token_expiry($token_expire)
					->set_firstname($profile['given_name'])
					->set_surname($profile['family_name'])
					->set_link($profile['link'])
					->set_face($profile['picture'])
					->set_gender($profile['male'])
					->set_birthday($profile['birthday'])
					->set_locale($profile['locale'])
					->set_google_id($profile['id'])
					->save()->reload();
				$_SESSION['registration_id'] = $oUser->get_id();
				header("Location: /user/register");
			}
			
			exit;
		}
	}
	
	public function LoginAction(){
		if(count($_POST)>0){
			$log_in_success = user::factory()->attempt_login($_POST['username'],$_POST['password']);
			if($log_in_success){
				header("Location: /user/edit");
			}else{
				$this->_view->message = "Sorry, incorrect username or password";
				$this->_view->form_error = true;
			}
			
		}
	}
	
	public function RegisterAction(){
		if(count($_POST)>0){
			user::factory()
				->load_by_id($_SESSION['registration_id'])
				->set_username($_POST['username'])
				->set_password(crypt($_POST['password']))
				->save()
				->login();
			header("Location: /user/edit");
		}
	}
	
	public function EditAction(){
		$this->_view->user = user::get_current_user();
	}
	
	public function LogoutAction(){
		user::logout();
		header("Location: /");
		exit;
	}
	
	public function UpdateLatitudeFeedAction(){
		$this->_view->user = user::get_current_user();
		list($added, $ignored) = $this->_view->user->update_latitude_locations();
		\tcore\tcore::set_message("Added {$added} locations, ignored {$ignored} locations as duplicates.");
		header("Location: {$_SERVER['HTTP_REFERER']}");
		exit;
	}
	
	public function ViewDataFeedAction(){
		$this->_view->user = user::get_current_user();
		point::recalculate_point_order();
		$this->_view->feed = point::factory()
								->search()
									->where('uid', $this->_view->user->get_uid())
									->exec();

		var_dump($this->_view->feed);
		exit;
		// Get the deltas
		
	
		$db_name = \tcore\db::get_database_name();
		$deltas_select = "
				SELECT 
					pid,
					delta_lat, 
					delta_long, 
					delta_latlong,
					delta_timestamp_ms,
					delta_distance
				FROM {$db_name}.points_delta 
				WHERE 
					`uid` = {$this->_view->user->get_uid()};";
		echo $deltas_select;
		$deltas_result = \tcore\db::get_instance()->query($deltas_select);
		var_dump($deltas_result);
		$deltas = $deltas_result->fetchAll();
		$sec_per_hour = 60 * 60;
		foreach($deltas as $delta){
			$delta->delta_distance_miles 			= $delta->delta_distance * 3959;
			$delta->delta_distance_kilometers 		= $delta->delta_distance * 6371;
			$delta->delta_distance_miles_nautical 	= $delta->delta_distance * 3440;
			$delta->miles_per_hour 					= ($delta->delta_distance_miles / $delta->delta_timestamp_ms) / $sec_per_hour;
			$delta->kilometers_per_hour 			= ($delta->delta_distance_kilometers / $delta->delta_timestamp_ms) / $sec_per_hour;
			$this->_view->deltas[$delta->pid] 		= $delta;
		}
	}
	
	public function CronAction(){
		$users = user::get_all_users();
		foreach($users as $user){
			echo "Processing {$user->get_firstname()} {$user->get_surname()}\n";
			
			// Update the users refresh token
			/*try{
				echo "Attempting to refresh Google Auth token:";
				$user->refresh_token();
				echo "OKAY";
			}catch(Exception $e){
				echo $e->getMessage()."\n\n";
			}*/
			
			// Get their locations
			try{
				$user->update_latitude_locations();
				point::recalculate_point_order();
			}catch(Exception $e){
				$e->getmessage()."\n";
			}
			echo "\n\n";
		}
		exit;
	}
}