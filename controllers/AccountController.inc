<?php

class AccountController extends BaseController {

  public function IndexAction(){}

  public function EditAction(){}

  public function GoogleAction(){}

  public function TwitterAction(){}

  public function _FoursquareInclude(){
    // Good grief.
    require_once("../lib/foursquare-php/lib/TheTwelve/Foursquare/ApiGatewayFactory.php");
    require_once("../lib/foursquare-php/lib/TheTwelve/Foursquare/EndpointGateway.php");
    require_once("../lib/foursquare-php/lib/TheTwelve/Foursquare/AuthenticationGateway.php");
    require_once("../lib/foursquare-php/lib/TheTwelve/Foursquare/CheckinsGateway.php");
    require_once("../lib/foursquare-php/lib/TheTwelve/Foursquare/HttpClient.php");
    require_once("../lib/foursquare-php/lib/TheTwelve/Foursquare/ListsGateway.php");
    require_once("../lib/foursquare-php/lib/TheTwelve/Foursquare/PhotosGateway.php");
    require_once("../lib/foursquare-php/lib/TheTwelve/Foursquare/TipsGateway.php");
    require_once("../lib/foursquare-php/lib/TheTwelve/Foursquare/UsersGateway.php");
    require_once("../lib/foursquare-php/lib/TheTwelve/Foursquare/VenueGroupsGateway.php");
    require_once("../lib/foursquare-php/lib/TheTwelve/Foursquare/VenuesGateway.php");
    require_once("../lib/foursquare-php/lib/TheTwelve/Foursquare/HttpClient/BuzzHttpClient.php");
    require_once("../lib/foursquare-php/lib/TheTwelve/Foursquare/HttpClient/HttpClientAdaptor.php");
    require_once("../lib/foursquare-php/lib/TheTwelve/Foursquare/HttpClient/SymfonyHttpClient.php");
  }
  /**
   * @return \TheTwelve\Foursquare\ApiGatewayFactory
   */
  public function _FoursquareGetApiFactory(){
    $this->_FoursquareInclude();
    $client = new \TheTwelve\Foursquare\HttpClient\SymfonyHttpClient();
    $factory = new \TheTwelve\Foursquare\ApiGatewayFactory($client);

    // Required for most requests
    $factory->setClientCredentials(FOURSQUARE_ID, FOURSQUARE_SECRET);

    // Optional
    $factory->setEndpointUri('https://api.foursquare.com');
    $factory->useVersion(2);

    return $factory;
  }

  /**
   * @return \TheTwelve\Foursquare\AuthenticationGateway
   */
  public function _FoursquareGetAuthenticationGateway(){
    $auth = $this->_FoursquareGetApiFactory()->getAuthenticationGateway(
      'https://foursquare.com/oauth2/authorize',
      'https://foursquare.com/oauth2/access_token',
      'https://gamitu.de/Account/Foursquare-Attach-Complete'
    );
    return $auth;
  }

  public function FoursquareAction(){}

  public function FoursquareAttachAction(){
    $auth = $this->_FoursquareGetAuthenticationGateway();
    $auth->initiateLogin();
  }

  public function FoursquareAttachCompleteAction(){
    $code = $_GET['code'];

    // You should do some input sanitization to $code here, just in case
    $token = $auth->authenticateUser($code);
    var_dump($token);exit;
  }

  public function FoursquareShowPlacesAction(){
    $client = new \TheTwelve\Foursquare\HttpClient\SymfonyHttpClient();
  }

  public function DataAction(){}

  public function BadgesAction(){}

  public function StatsAction(){}

  public function ScheduledCheckinsAction(){
    $user                = user::get_current_user();
    $this->_view->events = scheduled_checkin_event::search()->where('uid', $user->get_uid())->exec();
  }
}