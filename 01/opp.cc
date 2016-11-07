#include <algorithm>
#include <iostream>
#include <locale>
#include <map>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

using namespace std;
using boost::lexical_cast;
using boost::regex_match;
using boost::wregex;
using boost::wsmatch;

// Type for storing fixed-point numbers (financial amounts, with 3-digit
// fractional part)
using value_t = long long;
// For consistency with names, we are using wstring for holding currencies
using currency_t = wstring;

// We assume max length of integer part of the number should be 6 digits
// It can't be longer, because we need to multiply those numbers
const unsigned INT_PART_LEN = 6;
// We assume max length of integer part of the number for local currency
// should be 15 digits. It doesn't need to multiply, so it can be longer
const unsigned INT_PART_LEN_LOCAL_CURR = 15;

const set<currency_t> CURRENCY_CODES = {
    L"AED", L"AFN", L"ALL", L"AMD", L"ANG", L"AOA", L"ARS", L"AUD", L"AWG",
    L"AZN", L"BAM", L"BBD", L"BDT", L"BGN", L"BHD", L"BIF", L"BMD", L"BND",
    L"BOB", L"BOV", L"BRL", L"BSD", L"BTN", L"BWP", L"BYR", L"BZD", L"CAD",
    L"CDF", L"CHE", L"CHF", L"CHW", L"CLF", L"CLP", L"CNY", L"COP", L"COU",
    L"CRC", L"CUC", L"CUP", L"CVE", L"CZK", L"DJF", L"DKK", L"DOP", L"DZD",
    L"EGP", L"ERN", L"ETB", L"EUR", L"FJD", L"FKP", L"GBP", L"GEL", L"GHS",
    L"GIP", L"GMD", L"GNF", L"GTQ", L"GYD", L"HKD", L"HNL", L"HRK", L"HTG",
    L"HUF", L"IDR", L"ILS", L"INR", L"IQD", L"IRR", L"ISK", L"JMD", L"JOD",
    L"JPY", L"KES", L"KGS", L"KHR", L"KMF", L"KPW", L"KRW", L"KWD", L"KYD",
    L"KZT", L"LAK", L"LBP", L"LKR", L"LRD", L"LSL", L"LTL", L"LVL", L"LYD",
    L"MAD", L"MDL", L"MGA", L"MKD", L"MMK", L"MNT", L"MOP", L"MRO", L"MUR",
    L"MVR", L"MWK", L"MXN", L"MXV", L"MYR", L"MZN", L"NAD", L"NGN", L"NIO",
    L"NOK", L"NPR", L"NZD", L"OMR", L"PAB", L"PEN", L"PGK", L"PHP", L"PKR",
    L"PLN", L"PYG", L"QAR", L"RON", L"RSD", L"RUB", L"RWF", L"SAR", L"SBD",
    L"SCR", L"SDG", L"SEK", L"SGD", L"SHP", L"SLL", L"SOS", L"SRD", L"SSP",
    L"STD", L"SVC", L"SYP", L"SZL", L"THB", L"TJS", L"TMT", L"TND", L"TOP",
    L"TRY", L"TTD", L"TWD", L"TZS", L"UAH", L"UGX", L"USD", L"USN", L"USS",
    L"UYI", L"UYU", L"UZS", L"VEF", L"VND", L"VUV", L"WST", L"XAF", L"XAG",
    L"XAU", L"XBA", L"XBB", L"XBC", L"XBD", L"XCD", L"XDR", L"XFU", L"XOF",
    L"XPD", L"XPF", L"XPT", L"XSU", L"XTS", L"XUA", L"XXX", L"YER", L"ZAR",
    L"ZMK", L"ZWL"};

///////////////// Sub-expressions used in the main regular expressions

static const wstring white_chars = L" \\t\\r\\n\\v\\f";
// We assume the following format of an amount <1-6 digits>,<1-3 digits>
// (Integer part length condition is checked later)
static const wstring float_number = L"([0-9]+)(?:\\,([0-9]{1,3})){0,1}";
// 3-uppercase letter currency symbol
static const wstring currency_name = L"([A-Z]{3})";
// static const wstring donor_name =
static const wstring donor_name = L"(\\X+)(?:[" + white_chars + L"]+)";

// Currency exchange rate regex (format:
// <whitespace><currency_symbol><whitespace><float><whitespace>)
const wregex regex_exchange_rate(L"^[" + white_chars + L"]*" + currency_name +
                                 L"[" + white_chars + L"]*" + float_number +
                                 L"[" + white_chars + L"]*");

// Donation regex (format:
// <whitespace><donor_name><whitespace><float>
// <whitespace><currency_symbol><whitespace>)
const wregex regex_donation(L"^[" + white_chars + L"]*" + donor_name +
                            float_number + L"[" + white_chars + L"]*" +
                            currency_name + L"[" + white_chars + L"]*");

// Range of donation regex (format:
// <whitespace><float><whitespace><float><whitespace>)
const wregex regex_range(L"^[" + white_chars + L"]*" + float_number + L"[" +
                         white_chars + L"]*" + float_number + L"[" +
                         white_chars + L"]*");

wsmatch match;

// Maps currency symbol to their exchange rates to local currency
map<currency_t, value_t> exchange_rate_by_currency;
// Maps value in local currency to donor to the full string representing 
// donation
multimap<value_t, wstring> donor_by_value;

///////////////// Utility functions

void print_error_message(int line_nr, wstring& line) {
    wcerr << "Error in line " << line_nr << ":" << line << endl;
}

// Trims whitespace from the end of the string 
wstring right_trim(wstring s) {
	return s.erase(s.find_last_not_of(L" \t\f\v\n\r") + 1);
}

// Returns Gaussian rounded mutiplication of two value_t numbers
value_t multiplication(value_t a, value_t b) {
    value_t result = a * b;
    int big_modulo = result % 2000;
    int small_modulo = result % 1000;
    if (small_modulo < 500)
        return result / 1000;
    else if (small_modulo > 500)
        return result / 1000 + 1;
    else if (big_modulo == 1500)
        return result / 1000 + 1;
    else  // big_modulo == 500
        return result / 1000;
}

// Converts two numeric strings into value_t, returns -1 on error
value_t number_from_double_string(wstring& before_dot, wstring& after_dot,
                                  unsigned int_part_len) {
    // We assume the integer part should have no more digits than
    // int_part_len and that integer part cannot have leading zeros
    if (before_dot.length() == 0 || before_dot.length() > int_part_len ||
        (before_dot.length() > 1 && before_dot[0] == '0'))
        return -1;

    value_t base_number = lexical_cast<value_t>(before_dot) * 1000;
    value_t extra_number =
        after_dot.length() == 0 ? 0 : lexical_cast<value_t>(after_dot);

    if (after_dot.size() == 0)
        return base_number;
    else if (after_dot.size() == 1)
        return base_number + extra_number * 100;
    else if (after_dot.size() == 2)
        return base_number + extra_number * 10;
    else if (after_dot.size() == 3)
        return base_number + extra_number;
    return -1;
}

// Inserts <currency,rate> into map
// returns -1 on failure, 0 on success
int process_exchange_rate(currency_t currency, value_t rate) {
    // it also checks for -1, signal of error from number_from_double_string()
    if (rate <= 0) return -1;

    if (CURRENCY_CODES.find(currency) == CURRENCY_CODES.end() ||
        exchange_rate_by_currency.find(currency) !=
            exchange_rate_by_currency.end())
        return -1;

    exchange_rate_by_currency[currency] = rate;
    return 0;
}

// Formats value_t number into wstring (with 3-digit fractional part)
wstring format_value(value_t value) {
    wstring frac_part = to_wstring(value % 1000);
    while (frac_part.length() != 3) frac_part = L'0' + frac_part;
    return to_wstring(value / 1000) + L',' + frac_part;
}

// Inserts <local_currency_value,donation_string> into multimap
// returns -1 on failure, 0 on success
int process_donation(wstring& donor, value_t value, currency_t currency) {
    if (value <= 0) return -1;
    // Check if rate for that specific currency has been added
    if (exchange_rate_by_currency.find(currency) ==
        exchange_rate_by_currency.end())
        return -1;
    value_t local_currency_value =
        multiplication(value, exchange_rate_by_currency[currency]);
    donor_by_value.emplace(
        local_currency_value,
        L'"' + right_trim(donor) + L"\",\"" +
            format_value(value) + L"\"," + currency);
    return 0;
}

// Finds all <range_begin,range_end> donations (inclusive) and prints them in
// ascending order (by local currency value)
void process_range(value_t range_begin, value_t range_end) {
    const auto itlow = donor_by_value.lower_bound(range_begin);
    const auto itup = donor_by_value.upper_bound(range_end);
    for (auto it = itlow; it != itup; ++it) wcout << (it->second) << L'\n';
}

// Assumptions:
//   1. We hold donations in a multimap with the following mapping:
//      donation_local_currency -> donation_string (a string in the format
//      specified in the assignment description). This fulfills
//      assignment requirements and could be easily replaced with another
//      implementation if neccessary.
//   2. We assume that as soon we get correct donation entry, we should no
//      longer parse currencies (the same applies to ranges and donations
//      case).
//   3. All ranges are processed online
int main() {
    // Setup locale to accept unicode strings
    setlocale(LC_ALL, "");
    locale loc("");
    wcin.imbue(loc);
    wcout.imbue(loc);
    wcerr.imbue(loc);

    bool parsed_rates = false, parsed_donations = false;
    int counter = 0;

    currency_t currency;
    wstring donor;
    wstring int_part, frac_part;

    wstring aux;
    while (getline(wcin, aux)) {
        counter++;

        // Parse currency exchange rates
        if (!parsed_rates) {
            if (regex_match(aux, match, regex_exchange_rate)) {
                currency = lexical_cast<currency_t>(match[1]);
                int_part = lexical_cast<wstring>(match[2]);
                frac_part = lexical_cast<wstring>(match[3]);

                value_t value = number_from_double_string(int_part, frac_part,
                                                          INT_PART_LEN);

                if (process_exchange_rate(currency, value) == -1) {
                    // Currency already inserted or with incorrect value
                    print_error_message(counter, aux);
                }
                continue;
            }
        }
        // Parse donations
        if (!parsed_donations) {
            if (regex_match(aux, match, regex_donation)) {
                donor = lexical_cast<wstring>(match[1]);
                int_part = lexical_cast<wstring>(match[2]);
                frac_part = lexical_cast<wstring>(match[3]);
                currency = lexical_cast<currency_t>(match[4]);

                value_t value = number_from_double_string(int_part, frac_part,
                                                          INT_PART_LEN);

                if (process_donation(donor, value, currency) == -1) {
                    print_error_message(counter, aux);
                } else {
                    parsed_rates = true;
                }
                continue;
            }
        }
        // Parse ranges
        {
            if (regex_match(aux, match, regex_range)) {
                int_part = lexical_cast<wstring>(match[1]);
                frac_part = lexical_cast<wstring>(match[2]);

                value_t range_begin = number_from_double_string(
                    int_part, frac_part, INT_PART_LEN_LOCAL_CURR);

                int_part = lexical_cast<wstring>(match[3]);
                frac_part = lexical_cast<wstring>(match[4]);

                value_t range_end = number_from_double_string(
                    int_part, frac_part, INT_PART_LEN_LOCAL_CURR);

                if (range_begin == -1 || range_end == -1 ||
                    range_begin > range_end) {
                    print_error_message(counter, aux);
                } else {
                    parsed_rates = true;
                    parsed_donations = true;

                    process_range(range_begin, range_end);
                }
                continue;
            }
        }
        // Line did not match any of the regexes
        print_error_message(counter, aux);
    }
    return 0;
}
