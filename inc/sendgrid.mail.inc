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
    $options = [
      'turn_off_ssl_verification' => FALSE,
      'protocol' => 'https',
      'port' => NULL,
      'url' => NULL,
      'raise_exceptions' => FALSE,
    ];
    // Create a new SendGrid object.
    $sendgrid = new SendGrid($key_secret, $options);
    $sendgrid_message = new SendGrid\Email();
    $sitename = variable_get('site_name');
    // Defining default unique args.
    $unique_args = [
      'id' => $message['id'],
      'module' => $message['module'],
    ];

    if (isset($message['params']['account']->uid)) {
      $unique_args['uid'] = $message['params']['account']->uid;
    }

    // Allow other modules to modify unique arguments.
    $args = module_invoke_all('sendgrid_integration_unique_args_alter', $unique_args);

    // Check if we got any variable back.
    if (!empty($args)) {
      $unique_args = $args;
    }

    // Checking if 'from' email-address already exist.
    if (isset($message['headers']['from']) || isset($message['headers']['From']) && $message['headers']['from'] = $message['headers']['From']) {
      $fromaddrarray = sendgrid_integration_parse_address($message['headers']['from']);
      $data['from'] = $fromaddrarray[0];
      $data['fromname'] = $fromaddrarray[1];
    }
    else {
      $data['from'] = variable_get('site_mail');
      $data['fromname'] = $sitename;
    }

    // Check if $send is set to be true.
    if ($message['send'] != 1) {
      watchdog('SendGrid Integration', 'Email was not sent because send value was disabled', NULL, WATCHDOG_NOTICE, $link = NULL);
      return TRUE;
    }
    // Build the Sendgrid mail object.
    // The message MODULE and ID is used for the Category. Category is the only
    // thing in the Sendgrid UI you can use to sort mail.

    // This is an array of categories for Sendgrid statistics.
    $categories = [
      $sitename,
      $message['module'],
      $message['id'],
    ];

    // Allow other modules to modify categories.
    $result = module_invoke_all('sendgrid_integration_categories_alter', $message, $categories);

    // Check if we got any variable back.
    if (!empty($result)) {
      $categories = $result;
    }

    $sendgrid_message
      ->setFrom($data['from'])
      ->setSubject($message['subject'])
      ->setCategories($categories)
      ->setUniqueArgs($unique_args);
    if (!empty($data['fromname'])) {
      $sendgrid_message->setFromName($data['fromname']);
    }
    if (isset($message['headers']['Reply-To'])) {
      $sendgrid_message->setReplyTo($message['headers']['Reply-To']);
    }

    // If there are multiple recipients we use a different method for To:
    if (strpos(',', $message['to'])) {
      $sendtosarry = explode(',', $message['to']);
      foreach ($sendtosarry as $item) {
        $firsttoaddress = sendgrid_integration_parse_address(array_shift($item));
        $sendgrid_message->addTo($firsttoaddress[0]);
        if (!empty($firsttoaddress[1])) {
          $sendgrid_message->addToName($firsttoaddress[1]);
        }
        $sendgrid_message->setSmtpapiTos($sendtosarry);
      }
    }
    else {
      $toaddrarray = sendgrid_integration_parse_address($message['to']);
      $sendgrid_message->addTo($toaddrarray[0]);
      if (!empty($toaddrarray[1])) {
        $sendgrid_message->addToName($toaddrarray[1]);
      }
    }

    //Add cc and bcc in mail if they exist.
    $cc_bcc_keys = ['cc', 'bcc'];
    $address_cc_bcc = [];
    foreach ($message['headers'] as $key => $value) {
      // handle latter case issue for cc and bcc key
      if (in_array(drupal_strtolower($key), $cc_bcc_keys)) {
        $mail_ids = explode(',', $value);
        foreach ($mail_ids as $mail_id) {
          list($mail_cc_address, $cc_name) = sendgrid_integration_parse_address($mail_id);
          $address_cc_bcc[drupal_strtolower($key)][] = [
            'mail' => $mail_cc_address,
            'name' => $cc_name,
          ];
        }
      }
    }
    if (array_key_exists('cc', $address_cc_bcc)) {
      foreach ($address_cc_bcc['cc'] as $item) {
        $sendgrid_message->addCc($item['mail']);
        $sendgrid_message->addCcName($item['name']);
      }
    }
    if (array_key_exists('bcc', $address_cc_bcc)) {
      foreach ($address_cc_bcc['bcc'] as $item) {
        $sendgrid_message->addBcc($item['mail']);
        $sendgrid_message->addBccName($item['name']);
      }
    }


    // Check Content-Type of message body.
    if (strpos($message['headers']['Content-Type'], 'text/html') !== FALSE) {
      $sendgrid_message->setHtml($message['body']);
    }
    else {
      // Message body is text/plain.
      $sendgrid_message->setText(drupal_wrap_mail(drupal_html_to_text($message['body'])));
    }

    // Prepare attachments.
    $attachments = [];
    if (isset($message['attachments']) && !empty($message['attachments'])) {
      foreach ($message['attachments'] as $attachmentitem) {
        if (is_file($attachmentitem)) {
          $attachments[$attachmentitem] = $attachmentitem;
        }
      }
    }

    // If we have attachments, add them.
    if (!empty($attachments)) {
      $sendgrid_message->setAttachments($attachments);
    }

    /* The Mime Mail module (mimemail) expects attachments as an array of file
    // arrays in $message['params']['attachments']. Mimemail is a very popular
    // module so we must adddress the way it handles attachments.
    if (isset($message['params']['attachments']) && !empty($message['params']['attachments'])) {
      foreach ($message['params']['attachments'] as $attachment) {
        // Sendgrid API only supports attachments as files.
        if (isset($attachment['uri'])) {
          $attachment_path = drupal_realpath($attachment['uri']);
          if (is_file($attachment_path)) {
            $sendgrid_message->setAttachments($attachment_path);
          }
        }
      }
      // Remove the file objects from $message['params']['attachments'].
      // (This prevents double-attaching in the drupal_alter hook below.)
      unset($message['params']['attachments']);
    }*/

    // Integration with the Maillog module to use for debugging.
    if (module_exists('maillog')) {
      if (variable_get('sendgrid_integration_maillog_log', TRUE)) {
        $record = new stdClass;

        $record->header_message_id = isset($message->MessageID) ? $message->MessageID : NULL;
        $record->subject = $sendgrid_message->getSubject();
        $record->header_from = $sendgrid_message->getFrom();
        // This returns an array of emails so we have to make a string.
        $record->header_to = implode(',', $sendgrid_message->getTos());
        $record->header_reply_to = $sendgrid_message->getReplyTo();
        $record->header_all = serialize($message['headers']);
        $record->sent_date = REQUEST_TIME;

        // Used to separate different portions of the body string.
        $divider = str_repeat('-', 60) . "\n";

        // @todo See issue #2656090 In development.
        // Load the attachments.
        $attachments = $sendgrid_message->getAttachments();

        // If there's more than one item to display, add a divider.
        if (!empty($attachments)) {
          $record->body = t('Body') . ":\n";
          $record->body .= $divider;
        }

        // Check Content-Type of message body and get the appropriate content.
        if (strpos($message['headers']['Content-Type'], 'text/html') !== FALSE) {
          $record->body .= 'Body HTML' . "\n";
          $record->body .= $sendgrid_message->getHtml();
          $bodydevel = 'Body HTML' . "\n" . $sendgrid_message->getHtml();
        }
        else {
          // Message body is text/plain.
          $record->body .= 'Body Plain Text' . "\n";
          $record->body .= $sendgrid_message->getText();
          $bodydevel = 'Body Plain Text' . "\n" . $sendgrid_message->getText();
        }

        // @todo This is waiting on issue #2656090
        // List the attachments.
        if (!empty($attachments)) {
          $record->body .= "\n";
          $record->body .= $divider;
          $record->body .= t('Attachments') . ":\n";
          $record->body .= $divider;
          foreach ($attachments as $file) {
            $record->body .= t('Filename') . ':' . $file['filename'] . "\n";
            $record->body .= t('Directory Name') . ':' . $file['dirname'] . "\n";
            $record->body .= t('Extension') . ':' . $file['extension'] . "\n";
            $record->body .= t('Basename') . ':' . $file['basename'] . "\n";
            $record->body .= "\n";
          }
        }

        drupal_write_record('maillog', $record);
      }

      // Display the e-mail using Devel module.
      if (variable_get('sendgrid_integration_maillog_devel', TRUE) && function_exists('dpm')) {
        $devel_msg = [];
        $devel_msg[t('Subject')] = $sendgrid_message->getSubject();
        $devel_msg[t('From')] = $sendgrid_message->getFrom();
        $devel_msg[t('To')] = implode(',', $sendgrid_message->getTos());
        $devel_msg[t('Reply-To')] = !empty($sendgrid_message->getReplyTo()) ? $sendgrid_message->getReplyTo() : NULL;
        $devel_msg[t('Headers')] = $sendgrid_message->getHeaders();
        $devel_msg[t('Body')] = $bodydevel;
        $devel_msg[t('Attachments')] = $sendgrid_message->getAttachments();

        dpm($devel_msg, 'maillog');
      }
    }


    // Lets try and send the message and catch the error.
    try {
      $response = $sendgrid->send($sendgrid_message);
    }
    catch (\SendGrid\Exception $e) {
      $error_code = filter_xss($e->getCode());
      watchdog('SendGrid Integration', 'Sending emails to Sengrind service failed with error code @error_code', ['@error_code' => $error_code], WATCHDOG_ERROR, $link = NULL);
      foreach ($e->getErrors() as $er) {
        $error_info = filter_xss($er);
        watchdog('SendGrid Integration', 'Sendgrid generated error @error_info', ['@error_info' => $error_info], NULL, WATCHDOG_ERROR, $link = NULL);
      }
      // Add message to queue if reason for failing was timeout or
      // another valid reason. This adds more error tolerance.
      $codes = [
        -110,
        404,
        408,
        500,
        502,
        503,
        504,
      ];
      if (in_array($error_code, $codes)) {
        $queue = DrupalQueue::get('SendGridResendQueue')->createItem($message);
      }
      return FALSE;
    }
    // Sanitize and store the response code for easy processing.
    $response_code = filter_xss($response->getCode());

    // Creating hook, allowing other modules react on sent email.
    module_invoke_all('sendgrid_integration_sent', $message['to'], $response_code, $unique_args, $response);

    if ($response_code = 200) {
      // If the code is 200 we are good to finish and proceed.
      return TRUE;
    }
    // Default to low. Sending failed.
    return FALSE;
  }

}
