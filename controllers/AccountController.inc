<?php

class AccountController extends BaseController {

  public function IndexAction(){}

  public function EditAction(){}

  public function GoogleAction(){}

  public function TwitterAction(){}

  public function FoursquareAction(){
    $user = user::get_current_user();
    $this->_view->foursquare_accounts = foursquare_account::search()->where('uid', $user->get_uid())->exec();
  }

  public function FoursquareAttachAction(){
    $fsq = new EpiFoursquare(FOURSQUARE_ID, FOURSQUARE_SECRET);
    $url = $fsq->getAuthorizeUrl(FOURSQUARE_REDIRECT_URL);
    header("Location: $url");
  }

  public function FoursquareAttachCompleteAction(){
    $user = user::get_current_user();
    $code = $_REQUEST['code'];
    $fsq = new EpiFoursquare(FOURSQUARE_ID, FOURSQUARE_SECRET);
    $token_response = $fsq->getAccessToken($code, FOURSQUARE_REDIRECT_URL);
    $foursquare_account = foursquare_account::search()->where('uid', $user->get_uid())->execOne();
    if(!$foursquare_account){
      $foursquare_account = new foursquare_account();
    }
    $response = json_decode($token_response->responseText);
    $foursquare_account->set_uid($user->get_uid());
    $foursquare_account->set_token($response->access_token);
    $foursquare_account->save();
    header("Location: " . u("Account","Foursquare"));
    exit;
  }

  public function FoursquareShowPlacesAction(){
    $user = user::get_current_user();
    $fsq = new EpiFoursquare(FOURSQUARE_ID, FOURSQUARE_SECRET, $user->get_foursquare_account()->get_token());
    $venue_history = $fsq->get("/users/self/venuehistory");
    var_dump($venue_history);exit;
  }

  public function DataAction(){}

  public function BadgesAction(){}

  public function StatsAction(){}

  public function ScheduledCheckinsAction(){
    $user                = user::get_current_user();
    $this->_view->events = scheduled_checkin_event::search()->where('uid', $user->get_uid())->exec();
  }
}