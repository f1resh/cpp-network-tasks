#pragma once
#include <boost/asio/ssl.hpp>
#include <string>

namespace ssl = boost::asio::ssl; // from <boost/asio/ssl.hpp>

namespace detail {

// The template argument is gratuituous, to
// allow the implementation to be header-only.
//
template<class = void>
void
load_root_certificates(ssl::context& ctx, boost::system::error_code& ec)
{
    std::string const cert =
            "-----BEGIN CERTIFICATE-----\n"
            "MIIDvzCCAqegAwIBAgIUGeJanKOuLvylG5IRdO22GmA/dxcwDQYJKoZIhvcNAQEL\n"
            "BQAwbzELMAkGA1UEBhMCU1IxDzANBgNVBAgMBlNlcmJpYTERMA8GA1UEBwwIQmVs\n"
            "Z3JhZGUxEjAQBgNVBAoMCU15Q29tcGFueTEoMCYGCSqGSIb3DQEJARYZcGF2ZWwu\n"
            "YWt1bG92Ljg4QGdtYWlsLmNvbTAeFw0yMzA0MDEwODU3NDBaFw0zMzAzMjkwODU3\n"
            "NDBaMG8xCzAJBgNVBAYTAlNSMQ8wDQYDVQQIDAZTZXJiaWExETAPBgNVBAcMCEJl\n"
            "bGdyYWRlMRIwEAYDVQQKDAlNeUNvbXBhbnkxKDAmBgkqhkiG9w0BCQEWGXBhdmVs\n"
            "LmFrdWxvdi44OEBnbWFpbC5jb20wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\n"
            "AoIBAQDdv0WlJhkA/FuaWiCiGG6vT6p0wDk4jWhpWvrCYuTDwpeA55xeYS4kyRzW\n"
            "H70HMmnUZv7cWkQE2ZnPvfCbavK9najhdQpOY9EyboVVJynRfYaPUaMer/4gLrtg\n"
            "rYcoaDp1Q+4JPiG5aJ443UFmVjQowW71RbYbQxwZgO38WhKu/BLjnro+xHNnKIEC\n"
            "lv4Rw5jclGeGC1BtW+PDbML5eDDswKz+pJYu8lYh9s9m+zqu5vqHdyzSVmcPpzGz\n"
            "tvz22qvMLEuh3QaD6+MqrH9kSwb013XnUIGQUye9ERfEPP6tk1hD5LIquWbTLKl4\n"
            "/B/Fwe+PeZcabRO7+9nsbRM5gAsLAgMBAAGjUzBRMB0GA1UdDgQWBBQt3wTLjoyb\n"
            "fSktpMfjpj/4m6WSHzAfBgNVHSMEGDAWgBQt3wTLjoybfSktpMfjpj/4m6WSHzAP\n"
            "BgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQA9a9fBCl+y7yIc2bfu\n"
            "21/PBzvWMyYo3aH6xzfEnQQ2anqbqk02LaI5rBE+6dJo2ehMiW9EQk1ID2HWF6Yk\n"
            "ilRNKwI3iSDja+NzCWiyzPuX1pfziTI/O3Sec2HCaOWSg3r2ANZG1mZnmLtbllEE\n"
            "Sk0dJ49JP0x+wj62ExzL4k74Awx4yc53OhP9FiUcZB8JCrx6t3itUzLBl3Ee6W9z\n"
            "eKh0muDqtlc08tOqWpmt3j5UoIsHBJ38ROUxNJaL++/6K/U0wY6uvN1GYWy0WTaD\n"
            "eiqxymEJUU83iSH+No2oorL6gEgMR4fhA4jr7S29BuXVgx9/5ixSFL2MzUqr2q7K\n"
            "U0Ew\n"
            "-----END CERTIFICATE-----\n"
        ;

    ctx.add_certificate_authority(
        boost::asio::buffer(cert.data(), cert.size()), ec);
    if(ec)
        return;
}

} // detail

// Load the root certificates into an ssl::context
//
// This function is inline so that its easy to take
// the address and there's nothing weird like a
// gratuituous template argument; thus it appears
// like a "normal" function.
//

inline
void
load_root_certificates(ssl::context& ctx, boost::system::error_code& ec)
{
    detail::load_root_certificates(ctx, ec);
}

inline
void
load_root_certificates(ssl::context& ctx)
{
    boost::system::error_code ec;
    detail::load_root_certificates(ctx, ec);
    if(ec)
        throw boost::system::system_error{ec};
}
