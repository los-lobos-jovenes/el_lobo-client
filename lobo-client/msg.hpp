#ifndef _MSG_HPP
#define _MSG_HPP

#include <string>
#include <vector>
#include <sstream>

// Config entry point
#ifndef MSG_SEPARATOR
#define MSG_SEPARATOR '\31'
#endif

class msg
{

        std::vector<std::string> parts;
        static constexpr char separator = MSG_SEPARATOR;

public:
        msg() = default;

        template <typename T>
        msg &form(const T &part)
        {
                parts.push_back(part);
                return *this;
        }

        template <typename T, typename... Targs>
        msg &form(const T &part, Targs... parts)
        {
                this->parts.push_back(part);
                form(parts...);
                return *this;
        }

        std::string concat() const
        {
                std::string sum = "";
                sum += separator;

                for (auto i : parts)
                {
                        sum += i;
                        sum += separator;
                }
                return sum;
        }

        std::string operator[](std::size_t no) const
        {
                return this->extract(no);
        }

        std::string extract(std::size_t no) const
        {
                if (no < parts.size())
                {
                        return parts[no];
                }
                return "";
        }

        msg &decode(std::string s)
        {
                std::stringstream ss;
                ss << s;
                std::string part;

                while (getline(ss, part, separator))
                {
                        this->parts.push_back(part);
                }
                return *this;
        }

};

#undef MSG_SEPARATOR

#endif
