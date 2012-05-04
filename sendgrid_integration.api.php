<?php

/**
 * @file
 * Hooks provided by SendGrid Integration module
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * This hook is invoked after email has been sent.
 *
 * @param string $to
 *   Address of email recipient
 *
 * @param integer $result_code
 *   http result code returned by drupal_http_request.
 *     - 2xx Request were successfull.
 *     - 4xx There were errors in parameters.
 *     - 5xx API call was unsuccessfull.
 *
 * @param array $unique_args
 *   Unique arguments used when email were sent, keyd by argument name.
 *     - id Message id
 *     - uid User id
 *     - module Module witch sended the message
 *
 * @param array $result_data
 *   Result data returned by drupal_http_request.
 */
function hook_sendgrid_integration_sent($to, $result_code, $unique_args, $result_data) {
  if($unique_args['module'] == 'my_module' && $result_code = 200) {
    watchdog('My Module', 'My module has successfully sent email', NULL, WATCHDOG_NOTICE, $link = NULL);
  }
}

/**
 * @} End of "addtogroup hooks".
 */
