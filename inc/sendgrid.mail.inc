<?php
/**
 * @file
 * Implements Drupal MailSystemInterface
 */

class SendGridMailSystem implements MailSystemInterface {

  /**
   * Email formatting, example strip away html
   */
  public function format(array $message) {
    // Join message array.
    $message['body'] = implode("\n\n", $message['body']);

    return $message;
  }

  /**
   * Send email message
   */
  public function mail(array $message) {
    // SendGrid authentication information.
    $server = 'https://sendgrid.com/api/mail.send.json';
    $user = variable_get('sendgrid_integration_username', '');
    $key = variable_get('sendgrid_integration_apikey', '');

    // SMTP API, we use these to identify messages.
    $smtp_api = array(
      'category' => variable_get('site_name', 'Drupal'),
      'unique_args' => array(
        'id' => $message['id'],
        'uid' => $message['params']['account']->uid,
        'module' => $message['module'],
      ),
    );

    // Data sended to sendgrid.
    $data = array(
      'api_user' => $user,
      'api_key' => $key,
      'x-smtpapi' => json_encode($smtp_api),
      'to' => $message['to'],
      'subject' => $message['subject'],
      'from' => variable_get('site_mail'),
      'fromname' => variable_get('site_name', 'Drupal'),
    );

    // Check Content-Type of message body.
    if (strpos($message['headers']['Content-Type'], 'text/plain') !== FALSE) {
      // Message body is text/plain.
      $data['text'] = drupal_wrap_mail(drupal_html_to_text($message['body']));
    }
    else {
      $data['html'] = $message['body'];
    }

    // Options for drupal_http_request.
    $options = array(
      'method' => 'POST',
      'data' => http_build_query($data),
      'timeout' => 30,
      'headers' => array(
        'Content-Type' => 'application/x-www-form-urlencoded',
      ),
    );

    $result = drupal_http_request($server, $options);

    $result_data = json_decode($result->data);
    // Creating hook, allowing other modules react on sent email.
    module_invoke_all('sendgrid_integration_sent', $message['to'], $result->code, $smtp_api['unique_args'], $result_data);

    // Check if sending message succeeded.
    if ($result_data->message == 'success') {
      return TRUE;
    }

    // Default response to message sending failed.
    // Create log entry.
    $errors = '';
    foreach ($result_data->errors as $error) {
      $errors .= $error . ' ';
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
