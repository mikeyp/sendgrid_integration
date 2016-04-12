SendGrid Integration for Drupal
--------------------------------------------------------------------------------
This project is not affiliated with SendGrid, Inc.

Use the issue tracker located at Drupal.org
(https://www.drupal.org/sendgrid_integration) for bug reports or questions
about this module.
If you want more info about SendGrid services, contact SendGrid
(https://sendgrid.com).

FUNCTIONALITY
--------------------------------------------------------------------------------
This module overrides default email sending behaviour and sending emails through
SendGrid Transactional Email service instead.

Failures to send are re-queued for sending later. Queue of failed messages are
run on a 60 minute interval.

REQUIREMENTS
--------------------------------------------------------------------------------
Module dependencies:
Composer Manager (https://www.drupal.org/project/composer_manager) - A Drupal
module to faciltate the use of Composer (https://getcomposer.org) on a per-module
basis.

Composer is the definitive source for PHP dependencies, but implementing
Composer in modules is difficult to say the least. Composer typically acts over
an entire project - in this case it would be your entire Drupal source code. 
Composer manager allows for modules to declare their own composer.json file
rather than a project global composer.json.
  
  Composer is being used because of the usefulness it offers:
  https://www.acquia.com/blog/using-composer-manager-get-island-now

Running Composer and composer manager will require command line access via
DRUSH. There is no GUI for this tool.

Mailsystem - A module to create an agnostic management layer for Mail. Very
useful for controling the mail system on Drupal.

INSTALLATION
--------------------------------------------------------------------------------
Installing this module requires some more advanced knowledge, use of the command
line and the use of Drush.

1. Move this folder under modules directory of your installation,
   example sites/all/modules or sites/default/modules
   
2. Install Dependencies which include Composer Manager. Use Drush to update
   dependencies via composer `drush composer-manager update --no-dev`. 
   This will download the Sendgrid API. It is important to use the --no-dev option so you 
   do not download libariers that are used for development and testing only.
   
2. Navigate to Modules and enable SendGrid Integration in the Mail category.

3. Configure your SendGrid Username and API-Key in admin/config/system/sendgrid

4. Confirm that the mail system is setup to use Sendgrid for how you wish to run
   you website. If you want it all to run through Sendgrid then you set the
   System-wide default MailSystemInterface class to "SendGridMailSystem". As an
   example, see this [image](https://www.drupal.org/files/issues/sengrid-integration-mailsystem-settings-example.png).

* Composer Manager Documentation: [https://www.drupal.org/node/2405805](https://www.drupal.org/node/2405805)
* Composer Documentation: [https://getcomposer.org/doc/](https://getcomposer.org/doc/)

HTML Email
--------------------------------------------------------------------------------
In order to send HTML email. Your installation of Drupal must generate an email
with the proper headers. Sendgrid Integration modules looks for the content type
of the email to be set to "text/html" in the header (i.e. "Content-Type"="text/html").


OPTIONAL
--------------------------------------------------------------------------------
If sending email fails with certain (pre-defined) response codes the message be
added to Cron Queue for later delivery. In order for this to function, you must
configure Cron running period and when it is possible also add your drupal site
to crontab (Linux only), read more about cron at https://www.drupal.org/cron.

If you would like a record of the emails being sent by the website, installing
Maillog (https://www.drupal.org/project/maillog) will allow you to store local
copies of the emails sent. Sendgrid does not store the content of the email.

DEBUGGING
--------------------------------------------------------------------------------
Debugging this module while installed can be done by installing the Maillog
module (https://www.drupal.org/project/maillog). This module will allow you to
store the emails locally before they are sent and view the message generated
in the Sendgrid email object.

RESOURCES
--------------------------------------------------------------------------------
Information about the Sendgrid PHP Library is availabe on Github:
https://github.com/taz77/sendgrid-php-ng