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
				'http://gamitu.de/user/register-google', 
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
			$access = $auth->getAccess($_GET['code']);
			$userinfo = eden('google')->userinfo($access['access_token']);
			$profile = $userinfo->me();
			
			//First try to log them in with these credentials:
			$log_in_success = user::factory()->attempt_login_with_google($profile['id']);
			if($log_in_success instanceof user){
				$log_in_success->set_access_token($access['access_token'])->save();
				header("Location: /user/edit");
			}else{
				$oUser = user::factory()
					->set_access_token($access['access_token'])
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
		$this->_view->user->update_latitude_locations();
		//header("Location: /user/edit");
		exit;
	}
	
	public function ViewDataFeedAction(){
		$this->_view->user = user::get_current_user();
		$this->_view->feed = point::factory()
								->search()
									->where('uid',$this->_view->user->get_uid())
									->exec();
		
		// Get the deltas
		
		$db_name = \tcore\db::get_database_name();
		$sql_select = "SELECT * FROM {$db_name}.points_delta WHERE `uid` = {$this->_view->user->get_uid()};";
		$deltas = \tcore\db::get_instance()->query($sql_select)->fetchAll();
		$this->_view->deltas = $deltas;
		var_dump($deltas);exit;
	}
	
	public function CronAction(){
		$users = user::get_all_users();
		foreach($users as $user){
			echo "Processing {$user->get_firstname()} {$user->get_surname()}\n";
			try{
				$user->update_latitude_locations();
			}catch(Exception $e){
				$e->getmessage()."\n";
			}
			echo "\n\n";
		}
		exit;
	}
}