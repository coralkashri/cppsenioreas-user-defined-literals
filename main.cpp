/**
 * All examples are taken from C++ Senioreas blog
 * https://cppsenioreas.wordpress.com/
 *
 * Examples from:
 * cpp_senioreas::the_number_that_broke_and_spoke_cpp_investigator
 * https://cppsenioreas.wordpress.com/2020/10/13/the-number-that-broke-and-spoke-cpp-investigator/
 */

#include <iostream>
#include <chrono>
#include <complex>

#define EXAMPLE 3

using namespace std::literals::chrono_literals;
using namespace std::literals::complex_literals;

#if EXAMPLE == 1
void delay(std::chrono::nanoseconds) {}
#elif EXAMPLE == 2
class angle {
public:
    struct degrees {};
    struct radians {};
    constexpr angle(float deg, degrees) { _deg = deg; }
    constexpr angle(float rad, radians) { _deg = rad * 180 / M_PI; }
    [[nodiscard]] constexpr float get_degrees() const { return _deg; }
    [[nodiscard]] constexpr float get_radians() const { return _deg * M_PI / 180; }

private:
    float _deg;
};
#elif EXAMPLE == 3
struct date_offset {
    int d;
    int m;
    int y;

    constexpr date_offset(int days, int months, int years) : d(days), m(months), y(years) {}

    [[nodiscard]] constexpr date_offset operator+(date_offset ref) const {
        return date_offset(d + ref.d, m + ref.m, y + ref.y);
    }

    [[nodiscard]] constexpr date_offset operator-(date_offset ref) const {
        return *this + date_offset(-ref.d, -ref.m, -ref.y);
    }
};

class date {
public:
    constexpr date(int day, int month, int year)
            : d(day), m(month), y(year) {
        self_balance();
    }

    [[nodiscard]] constexpr date offset(date_offset offset_data) const {
        const auto after_years_offset = date(d, m, y + offset_data.y);
        const auto after_month_offset = date(d, m + offset_data.m, after_years_offset.get_year());
        return date(after_month_offset.get_day() + offset_data.d, after_month_offset.get_month(), after_month_offset.get_year());
    }

    [[nodiscard]] constexpr date operator+(date_offset offset_data) const {
        return offset(offset_data);
    }

    [[nodiscard]] constexpr unsigned short get_day() const { return d; }
    [[nodiscard]] constexpr unsigned short get_month() const { return m; }
    [[nodiscard]] constexpr unsigned short get_year() const { return y; }

private:
    int d, m, y;

    constexpr void self_balance() {
        unsigned short days_in_month;
        unsigned short days_in_prev_month = 31;
        bool is_change_detected = false;
        if (m == 2) {
            if (!(y % 4)) {
                days_in_month = 29;
            } else {
                days_in_month = 28;
            }
        } else {
            if (m <= 7 && m % 2 || m >= 8 && m % 2 == 0) {
                days_in_month = 31;
                if (m != 8) {
                    days_in_prev_month = 30;
                }
            } else {
                days_in_month = 30;
            }
        }

        if (d > days_in_month) {
            d -= days_in_month;
            m++;
            is_change_detected = true;
        } else if (d < 1) {
            d += days_in_prev_month;
            m--;
            is_change_detected = true;
        }

        if (m > 12) {
            m = 1;
            y++;
            is_change_detected = true;
        } else if (m < 1) {
            m = 12 - m;
            y--;
            is_change_detected = true;
        }

        if (is_change_detected) self_balance();
    }
};
#endif

namespace literals {
#if EXAMPLE == 2
    inline namespace angles_literals {
        constexpr angle operator "" _deg(long double deg) {
            return angle(deg, angle::degrees{});
        }

        constexpr angle operator "" _deg(unsigned long long int deg) {
            return angle(deg, angle::degrees{});
        }

        constexpr angle operator "" _rad(long double rad) {
            return angle(rad, angle::radians{});
        }

        constexpr angle operator "" _rad(unsigned long long int rad) {
            return angle(rad, angle::radians{});
        }
    }
#elif EXAMPLE == 3
    inline namespace dates_literals {
        constexpr date_offset operator"" _d(unsigned long long int days) {
            return date_offset(days, 0, 0);
        }

        constexpr date_offset operator"" _m(unsigned long long int months) {
            return date_offset(0, months, 0);
        }

        constexpr date_offset operator"" _y(unsigned long long int years) {
            return date_offset(0, 0, years);
        }
    }
#endif
}

using namespace literals;

int main() {
#if EXAMPLE == 1
    auto str = std::string_literals::operator""s("aaa", 3);
    auto initial_delay = 1001ns;
    delay(initial_delay);
    // ...
    auto increased_delay = 1s;
    delay(increased_delay);
    // ...
    auto current_delay = initial_delay + increased_delay;
    auto wait_time = 3min;
    delay(std::chrono::duration_cast<std::chrono::nanoseconds>(wait_time));
    // ...
    std::cout << "Total delay: " << std::chrono::duration_cast<std::chrono::seconds>(current_delay + wait_time).count() << " seconds." << std::endl;
#elif EXAMPLE == 2
    constexpr auto deg = 3.14159265_rad;
    constexpr auto rad = 360_deg;
    static_assert(deg.get_degrees() == 180);
    static_assert(rad.get_radians() == (float)(M_PI * 2));
    std::cout << deg.get_degrees() << std::endl;
    std::cout << deg.get_radians() << std::endl;
    std::cout << rad.get_degrees() << std::endl;
    std::cout << rad.get_radians() << std::endl;
#elif EXAMPLE == 3
    constexpr date my_date(23, 8, 2020);
    constexpr auto new_date = my_date + 8_d + 3_m + 5_y;
    std::cout << new_date.get_day() << " / " << new_date.get_month() << " / " << new_date.get_year() << std::endl;
    static_assert(new_date.get_day() == 1 && new_date.get_month() == 12 && new_date.get_year() == 2025);
#endif
    return EXIT_SUCCESS;
}
