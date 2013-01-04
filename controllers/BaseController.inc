<?php 

class BaseController{
	protected $_view;
	
	public function __construct($view){
		$this->_view = $view;
		header('Content-Type: text/plain',true);
	}
}