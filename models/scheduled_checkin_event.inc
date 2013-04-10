<?php 
use tcore\model;

class scheduled_checkin_event extends model{
	protected $_key = 'sce_id';
	protected $sce_id;
	protected $uid;
	
	protected $day_monday;
    protected $day_tuesday;
    protected $day_wednesday;
    protected $day_thursday;
    protected $day_fridayy;
    protected $day_saturday;
    protected $day_sunday;
    protected $probability;
    protected $fudge_minuites;
    protected $foursquare_location_id;
	
}