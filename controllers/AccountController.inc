<?php

class AccountController extends BaseController {

  public function IndexAction(){}

  public function EditAction(){}

  public function GoogleAction(){}

  public function TwitterAction(){}

  public function FoursquareAction(){}

  public function DataAction(){}

  public function BadgesAction(){}

  public function StatsAction(){}

  public function ScheduledCheckinsAction(){
    $user                = user::get_current_user();
    $this->_view->events = scheduled_checkin_event::search()->where('uid', $user->get_uid())->exec();
  }
}