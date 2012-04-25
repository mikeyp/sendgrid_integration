<?php

class SendGridMailSystem implements MailSystemInterface {
		
	/**
	 * Email formatting, example strip away html
	 */
	public function format(array $message) {
		// @todo: figure out formatting options
		return $message;
	}
	
	/**
	 * Send email message
	 */
	public function mail(array $message) {
		
		//SendGrid authentication information
		$server = 'https://sendgrid.com/api/mail.send.json';
		$user = '';
		$key = '';
		
		$smtp_api = array (
			'category' => variable_get('site_name', 'Drupal'),
			'unique_args' => array(
				'id' => $message['id'],
				'uid' => $message['params']['account']->uid,
				'module' => $message['module'],
			),
		);
		
		watchdog('sendgrid_integration', 'message: %code', array('%code' => print_r($message, TRUE)), WATCHDOG_NOTICE, $link = NULL);
		

		$data = array(
			'api_user' => $user,
			'api_key' => $key,
			'x-smtpapi' => json_encode($smtp_api),
			'to' => $message['to'],
			'subject' => $message['subject'],
			'html' => $message['body'][0],
			'text' => drupal_html_to_text($message['body'][0]),
			'from' => variable_get('site_mail'),
			'fromname' => variable_get('site_name', 'Drupal'),
		);
		
		
		watchdog('sendgrid_integration', 'data: %code', array('%code' => drupal_http_build_query($data, $parent = '')), WATCHDOG_NOTICE, $link = NULL);
				
    $options = array(
    	'method' => 'POST',
    	'data' => http_build_query($data),
    	'timeout' => 30,
    	'headers' => array(
    		'Content-Type' => 'application/x-www-form-urlencoded',
    	),
		);
		
		$result = drupal_http_request($server, $options);
		watchdog('sendgrid_integration', 'returned %code', array('%code' => print_r($result, true)), WATCHDOG_NOTICE, $link = NULL);
		
		return TRUE;
	}
}
