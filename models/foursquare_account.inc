<?php
use tcore\model;

class foursquare_account extends model {
  protected $_key = 'fsq_id';
  protected $fsq_id
  protected $uid;
  protected $_uid_type = self::TYPE_TEXT;
  protected $token;
}