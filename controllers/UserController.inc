<?php

class UserController extends BaseController {

  public function RegisterGoogleAction() {
    $scopes   = array(
      'userinfo',
      'latitude_all_best',
      'latitude_all_city',
      'latitude_current_best',
      'latitude_current_city'
    );
    $g_client = $this->get_google_client();
    $g_oauth2 = new Google_Oauth2Service($g_client);
    //$g_plus = new Google_PlusService($g_client);
    $g_latitude = new Google_LatitudeService($g_client);

    //if no code and no session
    if (!isset($_GET['code']) && !isset($_SESSION['token'])) {
      //redirect to login
      $authUrl = $g_client->createAuthUrl();
      header('Location: ' . $authUrl);
      exit;
    }

    //Code is returned back from google

    if (isset($_GET['code'])) {
      // save it to session
      $g_client->authenticate($_GET['code']);
      $user = $g_oauth2->userinfo->get();

      // Get access token
      $access = $g_client->getAccessToken();

      // These fields are currently filtered through the PHP sanitize filters.
      // See http://www.php.net/manual/en/filter.filters.sanitize.php
      $email = filter_var($user['email'], FILTER_SANITIZE_EMAIL);
      $img   = filter_var($user['picture'], FILTER_VALIDATE_URL);

      // First try to log them in with these credentials:
      $oUser = user::factory()->attempt_login_with_google($user['id']);

      // Decide if login or create user
      $login_or_create = ($oUser instanceof user) ? 'login' : 'create';

      // If create, create a new oUser
      if ($login_or_create == 'create') {
        $oUser = user::factory();
      }

      // Update the users records
      $oUser->update_access_token($access);
      $oUser->set_email($email);
      $oUser->set_firstname($user['given_name']);
      $oUser->set_surname($user['family_name']);
      $oUser->set_link($user['link']);
      $oUser->set_face($img);
      $oUser->set_gender($user['male']);
      $oUser->set_birthday($user['birthday']);
      $oUser->set_locale($user['locale']);
      $oUser->set_google_id($user['id']);
      $oUser->save()->reload();
      fb::log($user);
      fb::log($oUser);

      // Send onwards based on creation state.
      if ($login_or_create == "login") {
        header("Location: /user/edit");
      } else {
        $_SESSION['registration_id'] = $oUser->get_id();
        header("Location: /user/register");
      }
      exit;
    }
  }

  public function LoginAction() {
    if (count($_POST) > 0) {
      $log_in_success = user::factory()->attempt_login($_POST['username'], $_POST['password']);
      if ($log_in_success) {
        header("Location: /user/edit");
      } else {
        $this->_view->message    = "Sorry, incorrect username or password";
        $this->_view->form_error = true;
      }

    }
  }

  public function RegisterAction() {
    if (count($_POST) > 0) {
      user::factory()
        ->load_by_id($_SESSION['registration_id'])
        ->set_username($_POST['username'])
        ->set_password(crypt($_POST['password']))
        ->save()
        ->login();
      header("Location: /user/edit");
    }
  }

  public function EditAction() {
    $this->_view->user = user::get_current_user();
  }

  public function LogoutAction() {
    user::logout();
    header("Location: /");
    exit;
  }

  public function UpdateLatitudeFeed(user $user) {

    $g_client   = $this->get_google_client($user->get_access_token());
    $g_latitude = new Google_LatitudeService($g_client);
    $locations  = $g_latitude->location->listLocation(array('max-results' => 1000));
    $added      = 0;
    $ignored    = 0;
    foreach ($locations['items'] as $location) {
      $oNewPoint = new point();
      $oNewPoint->set_timestamp_ms($location['timestampMs']);
      $oNewPoint->set_latitude($location['latitude']);
      $oNewPoint->set_longitude($location['longitude']);
      $oNewPoint->set_uid($user->get_uid());
      $human_time = date("Y-m-d H:i:s", $oNewPoint->get_timestamp_ms());

      if (!$oNewPoint->already_exists()) {
        $oNewPoint->save();
        echo "Adding point {$oNewPoint->get_uid()}:{$oNewPoint->get_latitude()}/{$oNewPoint->get_longitude()}@{$human_time} ";
        echo "Saved as {$oNewPoint->get_pid()}\n";
        $added++;
      } else {
        $ignored++;
      }
    }
    echo "Added {$added}, ignored {$ignored}\n\n";
    return array($added, $ignored);
  }

  public function UpdateLatitudeFeedAction() {
    $oUser = user::get_current_user();
    //list($added, $ignored) = $oUser->update_latitude_locations();
    list($added, $ignored) = $this->UpdateLatitudeFeed($oUser);
    $this->_view->user = $oUser;
    \tcore\tcore::set_message("Added {$added} locations, ignored {$ignored} locations as duplicates.");
    header("Location: {$_SERVER['HTTP_REFERER']}");
    exit;
  }

  public function ViewDataFeedAction() {
    $this->_view->user = user::get_current_user();
    point::recalculate_point_order();
    $this->_view->feed = point::factory()
      ->search()
      ->where('uid', $this->_view->user->get_uid())
      ->exec();

    // Get the deltas
    $db_name       = \tcore\db::get_database_name();
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
    foreach ($deltas as $delta) {
      $delta->delta_distance_miles          = $delta->delta_distance * 3959;
      $delta->delta_distance_kilometers     = $delta->delta_distance * 6371;
      $delta->delta_distance_miles_nautical = $delta->delta_distance * 3440;
      $delta->miles_per_hour                = ($delta->delta_distance_miles / $delta->delta_timestamp_ms) / $sec_per_hour;
      $delta->kilometers_per_hour           = ($delta->delta_distance_kilometers / $delta->delta_timestamp_ms) / $sec_per_hour;
      $this->_view->deltas[$delta->pid]     = $delta;
    }
  }

  public function CronAction() {
    $users = user::get_all_users();
    foreach ($users as $user) {
      $user = user::cast($user);

      echo "Processing {$user->get_firstname()} {$user->get_surname()}\n";

      echo " > Refresh Token... ";
      try{
        $g_client = $this->get_google_client($user->get_access_token());
      }catch(Google_AuthException $e){
        echo "FAIL: Auth token invalid\n";
        $g_client = false;
      }
      if($g_client){
        $user->update_refresh_token($g_client);
        echo "OKAY\n";

        // Get their locations
        echo " > Update Locations... ";
        try{
          $this->UpdateLatitudeFeed($user);
          point::recalculate_point_order();
          $latest_point = $user->get_last_location();
          echo "Last seen at {$latest_point->get_town()}\n";
        }catch(Exception $e){
          $e->getmessage()."\n";
        }
      }
      echo "\n\n";

    }
    exit;
  }

  public function IndexAction(){
    
  }
}