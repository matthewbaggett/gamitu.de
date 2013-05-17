<?php

class MapController extends BaseController {
  public function ViewLocationAction() {
    $ll                = explode(",", $_GET['ll'], 2);
    $this->_view->lat  = $ll[0];
    $this->_view->long = $ll[1];
  }

  public function ViewJourneyAction(){
    if(isset($_GET['start'])){
      $start = strtotime($_GET['start']);
    }else{
      $start = strtotime("24 hours ago");
    }

    if(isset($_GET['end'])){
      $end = strtotime($_GET['end']);
    }else{
      $end = time();
    }

    if(isset($_GET['user_id'])){
      $user_id = intval($_GET['user_id']);
      $interrogated_user = user::search()->where('uid', $user_id)->execOne();
    }else{
      $interrogated_user = user::get_current_user();
    }

    $str_start  = date("Y-m-d H:i:s", $start);
    $str_end    = date("Y-m-d H:i:s", $end);

    $sql_pid_seek = "
      SELECT
        pid
      FROM
        points_view
      WHERE
        uid = {$interrogated_user->get_uid()}
        AND time_human < '{$str_end}'
        AND time_human > '{$str_start}'
      ";

    $pids_results = \tcore\db::get_instance()->query($sql_pid_seek);

    $this->_view->pids = array();

    foreach($pids_results->fetchAll() as $pid_result){
      $this->_view->pids[] = $pid_result->pid;
    }

    $this->_view->points = point::search()
      ->where('pid', $this->_view->pids, 'IN')
      ->order('timestamp_ms', 'ASC')
      ->exec();

    $this->_view->user = $interrogated_user;
    $this->_view->start = $start;
    $this->_view->end = $end;

  }
}