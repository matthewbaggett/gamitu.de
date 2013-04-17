<?php 

class BaseController{
	protected $_view;
	
	public function __construct($view){
		$this->_view = $view;
		header('Content-Type: text/plain',true);
		
		if(method_exists($this,"AllActions")){
			$this->AllActions();
		}
	}

  /**
   * Get a an instance of our Google Client
   * @return Google_Client
   */
  public function get_google_client(){
    $client = new Google_Client();
    $client->setApplicationName(APPLICATION_NAME);
    // Visit https://code.google.com/apis/console?api=plus to generate your
    // client id, client secret, and to register your redirect uri.
    $client->setClientId(GOOGLE_CLIENT_ID);
    $client->setClientSecret(GOOGLE_CLIENT_SECRET);
    $client->setRedirectUri(GOOGLE_LOGIN_REDIRECT);
    $client->setDeveloperKey(GOOGLE_API_KEY);
    $client->setAccessType('offline');
    return $client;

  }
}