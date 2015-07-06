<?php

/**
 * @file
 * Implements Drupal MailSystemInterface.
 */
class SendGridMailSystem implements MailSystemInterface {

  /**
   * Email formatting, example strip away html.
   */
  public function format(array $message) {
    // Join message array.
    $message['body'] = implode("\n\n", $message['body']);

    return $message;
  }

  /**
   * Send email message.
   */
  public function mail(array $message) {

    $key_secret = variable_get('sendgrid_integration_apikey', '');

    // Create a new SendGrid object.
    $sendgrid = new SendGrid($key_secret);
    $sendgrid_message = new SendGrid\Email();

    // Defining default unique args.
    $unique_args = array(
      'id' => $message['id'],
      'module' => $message['module'],
    );

    if (isset($message['params']['account']->uid)) {
      $unique_args['uid'] = $message['params']['account']->uid;
    }

    // Allow other modules to modify unique arguments.
    $args = module_invoke_all('sendgrid_integration_unique_args_alter', $unique_args);

    // Check if we got any variable back.
    if (!empty($args)) {
      $unique_args = $args;
    }

    // SMTP API, we use these to identify messages.
    $smtp_api = array(
      'unique_args' => $unique_args,
    );

    $parsed_to = sendgrid_integration_get_to($message['to']);
    $to_emails = $parsed_to['emails'];
    $to_names = $parsed_to['names'];

    // Data sended to sendgrid.
    $data = array(
      'to' => $to_emails,
      'subject' => $message['subject'],
    );

    if (!empty($to_names)) {
      $data['toname'] = $to_names;
    }

    // Checking if 'from' email-address already exist.
    if (isset($message['from'])) {
      list($from_address, $from_name) = sendgrid_integration_parse_address($message['from']);
      $data['from'] = $from_address;
      $data['fromname'] = $from_name;
    }
    else {
      $data['from'] = variable_get('site_mail');
      $data['fromname'] = variable_get('site_name');
    }

    // Check Content-Type of message body.
    if (strpos($message['headers']['Content-Type'], 'text/plain') !== FALSE) {
      // Message body is text/plain.
      $data['text'] = drupal_wrap_mail(drupal_html_to_text($message['body']));
    }
    else {
      $data['html'] = $message['body'];
    }

    // Check if $send is set to be true.
    if ($message['send'] != 1) {
      watchdog('SendGrid Integration', 'Email was not sent because send value was disabled', NULL, WATCHDOG_NOTICE, $link = NULL);
      return TRUE;
    }
    // Build the Sendgrid mail object.
    // The message MODULE and ID is used for the Category. Category is the only
    // thing in the Sendgrid UI you can use to sort mail.
    $sitename =  variable_get('site_name');
    // This is an array of categories for Sendgrid statistics.
    $categories = array(
      $sitename,
      $message['module'],
      $message['id'],
    );
    $sendgrid_message
      ->setFrom($data['from'])
      ->setFromName($data['fromname'])
      ->setSubject($data['subject'])
      ->setHtml($data['html'])
      ->setText($date['text'])
      ->setCategories($categories)
      ->setUniqueArgs($unique_args);

    // If we have a To name we use it.
    if (!empty($data['toname'])) {
      $sendgrid_message->addTo($data['to'], $data['toname']);
    }
    else {
      $sendgrid_message->addTo($data['to']);
    }

    // Lets try and send the message and catch the error.
    try {
      $response = $sendgrid->send($sendgrid_message);
    }
    catch (\SendGrid\Exception $e) {
      $error_code = filter_xss($e->getCode());
      watchdog('SendGrid Integration', 'Sending emails to Sengrind service failed with error code @error_code', array('@error_code' => $error_code), WATCHDOG_ERROR, $link = NULL);
      foreach ($e->getErrors() as $er) {
        $error_info = filter_xss($er);
        watchdog('SendGrid Integration', 'Sendgrid generated error @error_info', array('@error_info' => $error_info), NULL, WATCHDOG_ERROR, $link = NULL);
      }
      // Add message to queue if reason for failing was timeout or
      // another valid reason. This adds more error tolerance.
      $codes = array(
        -110,
        404,
        408,
        500,
        502,
        503,
        504,
      );
      if (in_array($error_code, $codes)) {
        $queue = DrupalQueue::get('SendGridResendQueue')->createItem($message);
      }
      return FALSE;
    }
    // Sanitize and store the response code for easy processing.
    $response_code = filter_xss($response->getCode());

    // @TODO Old method to remove.

    // Creating hook, allowing other modules react on sent email.
    module_invoke_all('sendgrid_integration_sent', $message['to'], $response_code, $smtp_api['unique_args'], $response);

    if ($response_code = 200) {
      // If the code is 200 we are good to finish and proceed.
      return TRUE;
    }

    return FALSE;
  }

}
