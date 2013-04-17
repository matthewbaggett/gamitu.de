<?php 
use tcore\model;

class scheduled_checkin_event extends model{
	protected $_key = 'sce_id';
	protected $sce_id;
	protected $uid;
    protected $_uid_type                    = model::TYPE_INT;
	
	protected $day_monday;
    protected $_day_monday_type             = model::TYPE_ON_OFF;
    protected $day_tuesday;
    protected $_day_tuesday_type            = model::TYPE_ON_OFF;
    protected $day_wednesday;
    protected $_day_wednesday_type          = model::TYPE_ON_OFF;
    protected $day_thursday;
    protected $_day_thursday_type           = model::TYPE_ON_OFF;
    protected $day_friday;
    protected $_day_friday_type             = model::TYPE_ON_OFF;
    protected $day_saturday;
    protected $_day_saturday_type           = model::TYPE_ON_OFF;
    protected $day_sunday;
    protected $_day_sunday_type             = model::TYPE_ON_OFF;
    protected $probability;
    protected $_probability_type            = model::TYPE_INT;
    protected $start_hour;
    protected $_start_hour_type             = model::TYPE_INT;
    protected $start_minute;
    protected $_start_minute_type           = model::TYPE_INT;
    protected $end_hour;
    protected $_end_hour_type               = model::TYPE_INT;
    protected $end_minute;
    protected $_end_minute_type             = model::TYPE_INT;
    protected $foursquare_location_id;
    protected $_foursquare_location_id_type = model::TYPE_STR;
	
}