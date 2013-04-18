<?php

class MapController extends BaseController {
  public function ViewAction() {
    $user                = user::get_current_user();
    $this->_view->events = scheduled_checkin_event::search()->where('uid', $user->uid)->exec();
  }

  public function ScheduledAction() {

  }

  public function AttachAction() {
  }
}