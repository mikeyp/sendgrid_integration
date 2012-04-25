<?php

class SendGridMailSystem implements MailSystemInterface {
		
	/**
	 * Email formatting, example strip away html
	 */
	public function format(array $message) {
		
		$message['body'] = implode("\n\n", $message['body']);
		
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
		
		//SMTP API, we use these to identify messages
		$smtp_api = array (
			'category' => variable_get('site_name', 'Drupal'),
			'unique_args' => array(
				'id' => $message['id'],
				'uid' => $message['params']['account']->uid,
				'module' => $message['module'],
			),
		);
		
		//Data sended to sendgrid
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
			
		//Options for drupal_http_request		
    $options = array(
    	'method' => 'POST',
    	'data' => http_build_query($data),
    	'timeout' => 30,
    	'headers' => array(
    		'Content-Type' => 'application/x-www-form-urlencoded',
    	),
		);
		
		$result = drupal_http_request($server, $options);
		
		//Check if sending message succeeded
		if($result->code == 200) {
			return TRUE;
		}		
		// Default response to message sending failed
		
		// Create log entry
		$result_data = json_decode($result->data);
		$errors = '';
		foreach ($result_data->errors as $error) {
			$errors .= $error.' ';
		}
		
		$variables = array(
			'%code' => $result->code, 
			'%msg' => $result->error,
			'%errors' => $errors,
		);
		watchdog('SendGrid Integration', 'Email sending failed with %code/%msg. %errors', $variables, WATCHDOG_NOTICE, $link = NULL);
				
		return FALSE;
	}
}
