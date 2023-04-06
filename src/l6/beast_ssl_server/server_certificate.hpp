#pragma once

#include <boost/asio/buffer.hpp>
#include <boost/asio/ssl/context.hpp>
#include <cstddef>
#include <memory>

/*  Load a signed certificate into the ssl context, and configure
    the context for use with a server.

    For this to work with the browser or operating system, it is
    necessary to import the "Beast Test CA" certificate into
    the local certificate store, browser, or operating system
    depending on your environment Please see the documentation
    accompanying the Beast certificate for more details.
*/
inline
void
load_server_certificate(boost::asio::ssl::context& ctx)
{
    /*
        The certificate was generated from CMD.EXE on Windows 10 using:

        winpty openssl dhparam -out dh.pem 2048
        winpty openssl req -newkey rsa:2048 -nodes -keyout key.pem -x509 -days 10000 -out cert.pem -subj "//C=US\ST=CA\L=Los Angeles\O=Beast\CN=www.example.com"
    */

    std::string const cert =
        "-----BEGIN CERTIFICATE-----\n"
"MIIDvzCCAqegAwIBAgIUFEqMW1HRrn/YOX85NUy+yHNNUYQwDQYJKoZIhvcNAQEL\n"
"BQAwbzELMAkGA1UEBhMCU1IxDzANBgNVBAgMBlNlcmJpYTERMA8GA1UEBwwIQmVs\n"
"Z3JhZGUxEjAQBgNVBAoMCU15Q29tcGFueTEoMCYGCSqGSIb3DQEJARYZcGF2ZWwu\n"
"YWt1bG92Ljg4QGdtYWlsLmNvbTAeFw0yMzAzMzEyMTEwNTdaFw0yNDAzMzAyMTEw\n"
"NTdaMG8xCzAJBgNVBAYTAlNSMQ8wDQYDVQQIDAZTZXJiaWExETAPBgNVBAcMCEJl\n"
"bGdyYWRlMRIwEAYDVQQKDAlNeUNvbXBhbnkxKDAmBgkqhkiG9w0BCQEWGXBhdmVs\n"
"LmFrdWxvdi44OEBnbWFpbC5jb20wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\n"
"AoIBAQDdv0WlJhkA/FuaWiCiGG6vT6p0wDk4jWhpWvrCYuTDwpeA55xeYS4kyRzW\n"
"H70HMmnUZv7cWkQE2ZnPvfCbavK9najhdQpOY9EyboVVJynRfYaPUaMer/4gLrtg\n"
"rYcoaDp1Q+4JPiG5aJ443UFmVjQowW71RbYbQxwZgO38WhKu/BLjnro+xHNnKIEC\n"
"lv4Rw5jclGeGC1BtW+PDbML5eDDswKz+pJYu8lYh9s9m+zqu5vqHdyzSVmcPpzGz\n"
"tvz22qvMLEuh3QaD6+MqrH9kSwb013XnUIGQUye9ERfEPP6tk1hD5LIquWbTLKl4\n"
"/B/Fwe+PeZcabRO7+9nsbRM5gAsLAgMBAAGjUzBRMB0GA1UdDgQWBBQt3wTLjoyb\n"
"fSktpMfjpj/4m6WSHzAfBgNVHSMEGDAWgBQt3wTLjoybfSktpMfjpj/4m6WSHzAP\n"
"BgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQAmhpckyqNvwtePkHi9\n"
"W91LA0nmyHJPJJenabJp/xTopBl9eOzQRC+/tZHWxTCD3Gpx0WfZNum9ThX8flNe\n"
"DIx2XCD34L2qFr4sbyV4sJhbkF2f2NLmFpxUu0ut/pDaDWx2neVgV6REFFxXod3u\n"
"bMS+OugiZcNSIxTpVIggvOf04fBpwJwaoQTuP+wbeDrgyHFkNf0Gg7DWXmEn6tys\n"
"+71/aSAga7TzbbEUZpcg+9z8wAhQsd4yWUMMdTSfuZ0M/FsJ5iUQD6FmDg6DEmYR\n"
"CvnlAbtk7/tsP+O0Vr5auVM4au+LL9RirHf0NKks1bgdfCExkmcauNmLHJP9Qk06\n"
"ORKS\n"
"-----END CERTIFICATE-----\n";

    std::string const key =
            "-----BEGIN PRIVATE KEY-----\n"
            "MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDdv0WlJhkA/Fua\n"
            "WiCiGG6vT6p0wDk4jWhpWvrCYuTDwpeA55xeYS4kyRzWH70HMmnUZv7cWkQE2ZnP\n"
            "vfCbavK9najhdQpOY9EyboVVJynRfYaPUaMer/4gLrtgrYcoaDp1Q+4JPiG5aJ44\n"
            "3UFmVjQowW71RbYbQxwZgO38WhKu/BLjnro+xHNnKIEClv4Rw5jclGeGC1BtW+PD\n"
            "bML5eDDswKz+pJYu8lYh9s9m+zqu5vqHdyzSVmcPpzGztvz22qvMLEuh3QaD6+Mq\n"
            "rH9kSwb013XnUIGQUye9ERfEPP6tk1hD5LIquWbTLKl4/B/Fwe+PeZcabRO7+9ns\n"
            "bRM5gAsLAgMBAAECggEBAILWkfRg503ymOYZGboq6lwciHn81YZtwVawrNa3XT24\n"
            "eoY/or75Xi2sUIxzC6jMcS83WleQFyQ5dIzkCKzbTMDe3r+wvFUGhm/YUGmvAvit\n"
            "tRCpXbMEqtA4Yq0nlmEcJqn8W94uP93TQdBRlH4ajQojICIucMXjMhhGP6WvS+jc\n"
            "j9qOKWUR60m7omV783AHHQXnX/pRzuaOKAXhW3OG5KTQu+aEJtvlpFl4w/bjH1ar\n"
            "dIX0ADUJlrijDzam5kOi2mW0QRun4KYtULD0OoTv3Q4ljGU2XWM0kC14b3rhI3QB\n"
            "GjQe5nzCqEMTur8ziu7oaWDsYnsQr7WGENR7Gfhde7kCgYEA80sVLpokQ5bMeYyU\n"
            "bGiXjTE5K8Py11Huy1RfXnRJlnG4K1YAUbyEPf+BiO/KxAt5urrnzGgBzEP2SW06\n"
            "SQflFCSFPUyvmQrl0FOvpxf/6A0hvClkDGIa87+WHGgBoM5MELqMBzMGP+YN42zi\n"
            "p+TvITKb+w2Xs/JuMaWaYkHycj8CgYEA6VQcICrxVf7T6/dUF315eDPI4SRE8KrL\n"
            "dr8HjtgyvXAKgfffeg0YuVgza5uS59jC3I01AIHH6W/FD3uMSZ8LiCV//auIWUK5\n"
            "bm4CoL7+ekjwJWH9QY0/XRvKZ+Qn1apWw07XmDpP7U/GnMUspXgbqRabKLcPVhgE\n"
            "vaL/bZ9QnDUCgYAz7nHuCZ72AmlRBK3l2gmHbNtcOS5LwcRsCG129vGCOHHsZiYZ\n"
            "TFkZ9sEKnsy0PhLUxfKWkhjYJiWbFMKdNh8yxnzeUdElpMrbRhG3ZAsO2Ea8KEWf\n"
            "0w0XvGxpUdYPIhngdEFRhBWwbAXI5sQWvGpCts/Ord8U3QmidUvX72W19wKBgQDE\n"
            "zrRWkcI0xhgc7eaZVbXSpONovD4GPhoWlDUc0NGjplDGpbxzZ36WP4ZH2tbEBrr+\n"
            "a4DsZ4unJCa004JWSvfgI7lcz4XXvIdKVTKUQeXGYaVl8sw6sZhjweAtV24eNGqw\n"
            "nN0PQwhN5D9j1pC9FZ1HN5Eye45NET9WcZk/7uSAjQKBgAuIng+ktmumS9lZ4vBn\n"
            "5GI1Ab3lYGfU6ZVYocQhhhWrOzaAiG+ZqaxcMjk1cS04SGmRsDh0+4Yfli8Q5YJ5\n"
            "/smEUQucf+3cXEhwt3AAiUhKmXd/GgbbTWkW8K/ch0OP8ry44dM+GZbc+ipWSYA5\n"
            "Iw725vzcbzg1gF30wGXR3Lb8\n"
            "-----END PRIVATE KEY-----\n";

    std::string const dh =
            "-----BEGIN DH PARAMETERS-----\n"
            "MIIBCAKCAQEA3pPia17qga7PZM7t7JxAtfc3QLfpwT3/UDSUCPxkUyYzzbZxFE59\n"
            "VTF7D9pqv7TcSxWpoO3GxH7HBeQqnHUIXFTGkxJYsS8V4Qi1hkvsK7QASKdf/LqS\n"
            "+8jP4Xa5gFDyQc7oaH66/Yx1EDXj1y/B3y/1GxlcGbtvtE3RH7brKA3hzD5vCYE6\n"
            "n+P+6Ji9c1E7+LQmsDCLUmRjBBFHjWbzYXUd7ReBttsyR40HFU3Z1xV8hYKi84MK\n"
            "8xIm/ebANMmnYMkw/EL2ykrNHzwCwvocJ7ZYgYcXpXTzuM3EVF9i3CpDMZ0eHJTb\n"
            "cxU37mjMryXvbs/oSl3qFTlf2Re4++f+wwIBAg==\n"
            "-----END DH PARAMETERS-----\n";

   ctx.set_password_callback(
        [](std::size_t,
            boost::asio::ssl::context_base::password_purpose)
        {
            return "";
        });

    ctx.set_options(
        boost::asio::ssl::context::default_workarounds |
        boost::asio::ssl::context::no_sslv2 |
        boost::asio::ssl::context::single_dh_use);

    ctx.use_certificate_chain(
        boost::asio::buffer(cert.data(), cert.size()));

    ctx.use_private_key(
        boost::asio::buffer(key.data(), key.size()),
        boost::asio::ssl::context::file_format::pem);

    ctx.use_tmp_dh(
        boost::asio::buffer(dh.data(), dh.size()));
}
