<?php
use tcore\model;

class user extends model {
  protected $_key = 'uid';
  protected $uid;
  protected $username;
  protected $password;
  protected $email;
  protected $_email_type = self::TYPE_TEXT;
  protected $refresh_token;
  protected $_refresh_token_type = self::TYPE_TEXT;
  protected $access_token;
  protected $_access_token_type = self::TYPE_TEXT;
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

  /**
   * recast a user object so stupid IDEs don't have a waa waa
   *
   * @param user $user
   * @return user
   */
  static public function cast(user $user){
    return $user;
  }

  public function login() {
    $_SESSION['user'] = serialize($this);
  }

  public function logout() {
    unset($_SESSION['user']);
  }

  /**
   * Get the current user.
   *
   * @return user
   */
  static public function get_current_user() {
    if (self::is_loggedin()) {
      return unserialize($_SESSION['user']);
    } else {
      return false;
    }
  }

  /**
   * Are we logged in?
   *
   * @return bool
   */
  static public function is_loggedin() {
    if (isset($_SESSION['user'])) {
      if (unserialize($_SESSION['user']) instanceof user) {
        return true;
      }
    }

    return false;
  }

  static public function attempt_login($user, $password) {
    $oUser = self::factory()
      ->search()
      ->where('username', $user)
      ->execOne();
    if ($oUser) {
      if (crypt($password, $oUser->get_password()) == $oUser->get_password()) {
        $oUser->login();

        return true;
      } else {
        return false;
      }
    }

    return false;
  }

  static public function attempt_login_with_google($google_id) {
    $oUser = self::factory()
      ->search()
      ->where('google_id', $google_id)
      ->execOne();
    if ($oUser) {
      $oUser->login();

      return $oUser;
    }

    return false;
  }

  static public function get_all_users() {
    $users = user::factory()
      ->search()
      ->where('access_token', '', "!=")
      ->limit(50)
      ->exec();

    return $users;

  }

  /**
   * Get the last point the user was seen at
   * @return point
   */
  public function get_last_location() {
    $location = point::factory()
      ->search()
      ->where('uid', $this->get_uid())
      ->order('timestamp_ms', 'DESC')
      ->execOne();

    return $location;
  }

  public function get_oldest_location(){
    $location = point::factory()
      ->search()
      ->where('uid', $this->get_uid())
      ->order('timestamp_ms', 'ASC')
      ->execOne();

    return $location;
  }

  public function update_refresh_token(Google_Client $g_client) {
    $g_client->refreshToken($this->get_refresh_token());
    $this->update_access_token($g_client->getAccessToken());

    return $this;
  }

  public function update_access_token($access_token) {
    if (!is_object($access_token)) {
      $access_token = json_decode($access_token);
    }

    $encoded_token = json_encode($access_token);

    $this->set_access_token($encoded_token);
    $this->set_access_token_expiry(time() + $access_token->expires_in);
    $this->set_refresh_token($access_token->refresh_token);
    $this->save();

    return $this;
  }

  /**
   * @return foursquare_account
   */
  public function get_foursquare_account(){
    return foursquare_account::search()->where('uid', $this->get_uid())->execOne();
  }
}