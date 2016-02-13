======
 UNIX
======

If you have access to a linux shell the following applies:

The command within download_coinbase_cert can be run on a linux command line to download the coinbase certificate.  The file can be run as is or the command within can be modified to suit your needs.

The command within generate_coinbase_fingerprint can be run after download_coinbase_cert and a fingerprint will be generated as a result.  The file can be run as is or the command within can be modified to suit your needs.

Also, the fingerprint can be retrieved by the following command:
gnutls-cli -p 443 api.exchange.coinbase.com

=========
 WINDOWS
=========

If you have access to Google Chrome you can download the JSON data manually for instance: (api.exchange.coinbase.com/products/BTC-USD/book).  In chrome to the left of https:// there is a green lock. If you click on that and then click on the connection tab and then click on "certificate information" and finally on the details tab if you scroll all the way to the bottom there is a field called thumbprint. This is also called a fingerprint in other circles.

Additionally, this is a useful tool to find a fingerprint:
https://www.grc.com/fingerprints.htm

All methods above can be applied to any "HTTPS" website.