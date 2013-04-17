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
    $g_client = $this->get_google_client();
    $g_oauth2 = new Google_Oauth2Service($g_client);
    $g_plus = new Google_PlusService($g_client);
    $g_latitude = new Google_LatitudeService($g_client);
		
		//if no code and no session
		if(!isset($_GET['code']) && !isset($_SESSION['token'])) {
			//redirect to login
			$authUrl = $g_client->createAuthUrl();
			header('Location: '.$authUrl);
			exit;
		}
		
		//Code is returned back from google
		
		if(isset($_GET['code'])) {
			//save it to session
      $g_client->authenticate($_GET['code']);
      $user = $g_oauth2->userinfo->get();

      // These fields are currently filtered through the PHP sanitize filters.
      // See http://www.php.net/manual/en/filter.filters.sanitize.php
      $email = filter_var($user['email'], FILTER_SANITIZE_EMAIL);
      $img = filter_var($user['picture'], FILTER_VALIDATE_URL);

			// First try to log them in with these credentials:
			$log_in_success = user::factory()->attempt_login_with_google($user['id']);
			if($log_in_success instanceof user){
				$log_in_success->set_access_token($access['access_token'])->save();
				header("Location: /user/edit");
			}else{
				$token_expire = time() + $access['expires_in'];
				$oUser = user::factory()
					->set_access_token($access['access_token'])
					->set_access_token_expiry($token_expire)
          ->set_email($email)
					->set_firstname($user['given_name'])
					->set_surname($user['family_name'])
					->set_link($user['link'])
					->set_face($img)
					->set_gender($user['male'])
					->set_birthday($user['birthday'])
					->set_locale($user['locale'])
					->set_google_id($user['id'])
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

		// Get the deltas
		$db_name = \tcore\db::get_database_name();
		$deltas_select = "
				SELECT 
					pid1 as pid,
					lat_delta as delta_lat, 
					long_delta as delta_long, 
					time_delta as delta_timestamp_ms,
					distance_delta as delta_distance
				FROM {$db_name}.points_deltas  
				WHERE 
					`uid` = {$this->_view->user->get_uid()}
				LIMIT 50;
		";
		
		
		$deltas_result = \tcore\db::get_instance()->query($deltas_select);
		
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