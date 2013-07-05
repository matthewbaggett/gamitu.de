<?php

class MapController extends BaseController {
  const one_day_in_seconds = 86400;

  public function ViewLocationAction() {
    $ll                = explode(",", $_GET['ll'], 2);
    $this->_view->lat  = $ll[0];
    $this->_view->long = $ll[1];
  }

  public function ViewJourneyAction(){
    if(!isset($_SESSION['range'])){
      $_SESSION['range'] = self::one_day_in_seconds;

    }
    if(!isset($_SESSION['offset'])){
      $_SESSION['offset'] = 0;
    }

    if(count($_POST) > 0){
      if(isset($_POST['range'])){
        if($_POST['range'] == 'Day'){
          $_SESSION['range'] = self::one_day_in_seconds;
        }
        if($_POST['range'] == 'Week'){
          $_SESSION['range'] = self::one_day_in_seconds * 7;
        }
        if($_POST['range'] == 'Month'){
          $_SESSION['range'] = self::one_day_in_seconds * 28;
        }
        if($_POST['range'] == 'Year'){
          $_SESSION['range'] = self::one_day_in_seconds * 365;
        }
      }
      if(isset($_POST['offset'])){
        if($_POST['offset'] == 'Forwards'){
          $_SESSION['offset'] = $_SESSION['offset'] + $_SESSION['range'];
        }
        if($_POST['offset'] == 'Backwards'){
          $_SESSION['offset'] = $_SESSION['offset'] - $_SESSION['range'];
        }
      }

      // And redirect.
      $range_days = $_SESSION['range'] / self::one_day_in_seconds;
      $offset_days = $_SESSION['offset'] / self::one_day_in_seconds;
      header("Location: /map/view-journey?range={$range_days}&offset={$offset_days}");
      exit;
    }

    // Load offset & range from GET.
    if(isset($_GET['range']) && is_numeric($_GET['range'])){
      $_SESSION['range'] = $_GET['range'] * self::one_day_in_seconds;
    }
    if(isset($_GET['offset']) && is_numeric($_GET['offset'])){
      $_SESSION['offset'] = $_GET['offset'] * self::one_day_in_seconds;
    }

    $start  = time() + $_SESSION['offset'];
    $end    = time() + $_SESSION['offset'] + $_SESSION['range'];

    if(isset($_GET['user_id'])){
      $interrogated_user = user::search()->where('uid', $_GET['user_id'])->execOne();
    }
    if(!isset($interrogated_user)){
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
        AND changed_state = 'Yes'
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
    $this->_view->range_days = $_SESSION['range'] / self::one_day_in_seconds;
    $this->_view->offset_days = $_SESSION['offset'] / self::one_day_in_seconds;

  }
}