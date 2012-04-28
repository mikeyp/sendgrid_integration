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
 * This hook is invoked right after email has been sent.
 *
 * @param string $to
 *   Address of email recipient
 * @param integer $result_code
 *   http result code returned by drupal_http_request.
 *     - 2xx Request were successfull.
 *     - 4xx There were errors in parameters.
 *     - 5xx API call was unsuccessfull.
 * @param array $unique_args
 *   Unique arguments used when email were sent as array, keyd by argument name.
 *     - id Message id
 *     - uid User id
 *     - module Module witch sended the message
 * $paramn $result_data
 *   Result data returned by drupal_http_request as array.
 */
function hook_sendgrid_integration_sent($to, $result_code, $unique_args, $result_data) {

}

/**
 * @} End of "addtogroup hooks".
 */
