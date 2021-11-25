////
//// Created by Admin on 2021/11/24.
////
//
//#ifndef DEMOFFMPEG_STRINGUTILS_H
//#define DEMOFFMPEG_STRINGUTILS_H
//
//#include <sstream>
//#include <string>
//#include <vector>
//#include <algorithm>
//#include <stdarg.h>
//#include <stdlib>
//#include <cstdlib>
//#include <stdlib.h>
//#include "../../../../../../../../../android_sdk/android_sdk/sdk/ndk/21.1.6352462/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/c++/v1/string"
///**
//11 * Collection of various helper methods for strings.
//12 *
//13 * \sa std::string
//14 *
//15 * \todo    Test, test, test!
//16 */
//namespace StringUtils {
//
//    /**
//20     * Converts the string \a str to lowercase.
//21     * \param str   String to convert.
//22     * \return Lowercase version of \a str.
//23     */
//    static std::string lowercase(const std::string &str) {
//        std::string toReturn(str);
//        std::transform(toReturn.begin(), toReturn.end(), toReturn.begin(), ::tolower);
//        return toReturn;
//    };
//
//    /**
//30     * Converts the string \a str to uppercase.
//31     * \param str   String to convert.
//32     * \return Uppercase version of \a str.
//33     */
//    static std::string uppercase(const std::string &str) {
//        std::string toReturn(str);
//        std::transform(toReturn.begin(), toReturn.end(), toReturn.begin(), ::toupper);
//        return toReturn;
//    };
//
//    /**
//40     * Splits \a str into a vector of strings representing float values.
//41     * Floats formatted as [-][0-9]*.[0-9]* are considered, all other characters in between are ignored.
//42     *
//43     * \param   str     Input string to parse.
//44     *
//45     * \note    TODO: The detection algorithm is a litte simplified and will not yield correct results
//46     *          in every case.
//47     **/
//    static std::vector <std::string> parseFloats(const std::string &str) {
//        static const std::string floatCharacters("0123456789.-");
//        std::vector <std::string> toReturn;
//        size_t strpos = 0;
//        size_t endpos = 0;
//        // we just started or just finished parsing an entry, check if finished and skip to beginning of next entry
//        while ((endpos != std::string::npos) &&
//               (strpos = str.find_first_of(floatCharacters, strpos)) != std::string::npos) {
//            // strpos currently points to the beginning of a float, now find its end
//            endpos = str.find_first_not_of(floatCharacters, strpos + 1);
//            // extract float
//            std::string token = str.substr(strpos, endpos - strpos);
//            // sanity checks
//            size_t signPos = token.rfind('-');
//            if (signPos == 0 || signPos == std::string::npos) { // sign only allowed at beginning
//                if (token.find('.') == token.rfind('.')) {      // only one . allowed
//                    toReturn.push_back(token);
//                }
//            }
//            strpos = endpos + 1;
//        }
//        return toReturn;
//    };
//
//    /**
//71     * Replaces all occurrences of \a from in \a str with \a to.
//72     * \param str   String to perform replacement on.
//73     * \param from  String to be replaced.
//74     * \param to    String replace.
//75     * \return  \a str with all occurrences of \a from replaced with \a to.
//76     */
//    static std::string
//    replaceAll(const std::string &str, const std::string &from, const std::string &to) {
//        std::string toReturn(str);
//        std::string::size_type strpos = 0;
//        std::string::size_type foundpos;
//        while ((foundpos = toReturn.find(from, strpos)) != std::string::npos) {
//            toReturn.replace(foundpos, from.size(), to);
//            strpos = foundpos + to.size();
//        }
//        return toReturn;
//    };
//
//    static bool replace(std::string &str, const std::string &from, const std::string &to) {
//        size_t start_pos = str.find(from);
//        if (start_pos == std::string::npos)
//            return false;
//        str.replace(start_pos, from.length(), to);
//        return true;
//    };
//
//    static bool replaceLast(std::string &str, const std::string &from, const std::string &to) {
//        size_t start_pos = str.rfind(from);
//        if (start_pos == std::string::npos)
//            return false;
//        str.replace(start_pos, from.length(), to);
//        return true;
//    };
//
//    /**
//104     * Splits the string \a str into pieces separated by the delimiters in \a delimiter.
//105     * \param str       String to split.
//106     * \param delimiter Set of delimiters.
//107     * \return  Vector of the split substrings.
//108     */
//    static std::vector <std::string> split(const std::string &line, const std::string &delimiter) {
//        std::vector <std::string> toReturn;
//        std::string::size_type linepos = 0;
//        std::string::size_type endpos = 0;
//        // we are at the beginning of an entry, skip whitespace and check if not already reached end of line
//        while (endpos != std::string::npos) {
//            endpos = line.find_first_of(delimiter, linepos);
//            toReturn.push_back(line.substr(linepos, endpos - linepos));
//            linepos = endpos + 1;
//        }
//        return toReturn;
//    };
//
//    template<class T>
//    static std::vector <T> splitStringT(const std::string &line, const std::string &delimiter) {
//        std::vector <T> toReturn;
//        std::string::size_type linepos = 0;
//        std::string::size_type endpos = 0;
//        // we are at the beginning of an entry, skip whitespace and check if not already reached end of line
//        while (endpos != std::string::npos) {
//            endpos = line.find_first_of(delimiter, linepos);
//            toReturn.push_back(StringUtils::fromString<T>(line.substr(linepos, endpos - linepos)));
//            linepos = endpos + 1;
//        }
//        return toReturn;
//    };
//
//    /**
//135     * Trims the string \a str.
//136     * All leading and trailing occurrences of the characters in \a whitespace will be removed.
//137     *
//138     * \param str           The string to trim.
//139     * \param whitespace    Set of whitespace characters which shall be removed at the beginning and the end.
//140     * \return  The original string without leading and trailing whitespace.
//141     */
//    static std::string
//    trim(const std::string &str, const std::string &whitespace = " \t\n\r\0\x0B") {
//        std::string::size_type first = str.find_first_not_of(whitespace);
//        if (first == std::string::npos) {
//            return "";
//        } else {
//            std::string::size_type last = str.find_last_not_of(whitespace);
//            return str.substr(first, last - first + 1);
//        }
//    };
//
//    /**
//153     * Splits the string \a str into trimmed pieces separated by the delimiters in \a delimiter.
//154     * Delimiters in quoted strings (\a quotes) will be ignored, double quotes within quoted strings will be
//155     * interpreted as literal quotes. Each token will be trimmed.
//156     *
//157     * \param str           String to split.
//158     * \param delimiter     Set of delimiters.
//159     * \param quotes        Character used for quotes.
//160     * \param whitespace    Set of whitespace characters which shall be removed during trimming.
//161     * \return  Vector of the split substrings.
//162     */
//    static std::vector <std::string>
//    splitStringsafe(const std::string &str, const std::string &delimiter, char quotes = '"',
//                    const std::string &whitespace = " \t\n\r\0\x0B") {
//        std::vector <std::string> toReturn;
//        std::string::size_type strpos = 0;
//        std::string::size_type endpos = 0;
//        // we are at the beginning of an entry, skip whitespace and check if not already reached end of str
//        while ((endpos != std::string::npos) &&
//               (strpos = str.find_first_not_of(whitespace, strpos)) != std::string::npos) {
//            // now strpos points to the first non blank character, here starts the entry
//            // check whether there are quotes
//            if (str[strpos] == quotes) {
//                // find position of closing quotes
//                endpos = str.find_first_of('"', strpos + 1);
//                std::string toPush = str.substr(strpos + 1, endpos - strpos - 1);
//                // ensure we haven't found double quotes ("") which shall be resolved to one double quote in resulting string
//                while ((endpos != std::string::npos) && (endpos + 1 < str.length()) &&
//                       (str[endpos + 1] == '"')) {
//                    strpos = endpos + 1;
//                    endpos = str.find_first_of('"', endpos + 2);
//                    toPush.append(str.substr(strpos, endpos - strpos));
//                }
//                // push string in quotes onto toReturn
//                toReturn.push_back(trim(toPush, whitespace));
//                // ignore everything until next delimiter
//                endpos = str.find_first_of(delimiter, endpos);
//            }
//                // ok, this entry is not in quotes - just push everything until next delimiter onto toReturn
//            else {
//                endpos = str.find_first_of(delimiter, strpos);
//                toReturn.push_back(trim(str.substr(strpos, endpos - strpos), whitespace));
//            }
//            strpos = endpos + 1;
//        }
//        return toReturn;
//    };
//
//    /**
//198     * Converts the value \a value to a string.
//199     * \param value     The value to convert, must be compatible with std::stringstream.
//200     * \return  A string representation of \a value.
//201     */
//    template<class T>
//    static std::string toString(const T &value) {
//        std::ostringstream stream;
//        stream << value;
//        return stream.str();
//    };
//
//    /**
//209     * Converts the string \a str to its original value.
//210     * \param str   The string to convert
//211     * \return  The back-converted value of \a str, type must be compatible with std::stringstream.
//212     * \throw   tgt::Exception on conversion failure
//213     */
//    template<class T>
//    static T fromString(const std::string &str) {
//        T toReturn;
//        std::istringstream stream;
//        stream.str(str);
//        try {
//            stream >> toReturn;
//        }
//        catch (std::exception &ex) {
//            std::cout << "Failed to convert string '" << str << "'" << "  exception=" << ex.what()
//                      << std::endl;
//            return NULL;
//        }
//        return toReturn;
//    };
//
//    /**
//231     * Joins the substrings in \a tokens together using \a delimiter in between.
//232     * \param tokens    List of substrings to join.
//233     * \param delimiter Delimiter which shall be placed between the substrings.
//234     * \return  A string containing the joined substrings.
//235     */
//    template<typename T>
//    static std::string join(const std::vector <T> &tokens, const std::string &delimiter) {
//        if (tokens.empty())
//            return "";
//        std::stringstream s;
//        s << tokens[0];
//        for (std::vector<T>::size_type i = 1; i < tokens.size(); ++i)
//            s << delimiter << tokens[i];
//        return s.str();
//    };
//
//
//    static bool startsWith(const std::string &str, const std::string &prefix) {
//        return str.size() >= prefix.size() &&
//        str.compare(0, prefix.size(), prefix) == 0;
//    };
//
//    static bool endsWith(const std::string &str, const std::string &suffix) {
//        return str.size() >= suffix.size() &&
//               254
//        str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
//    };
//
//    static std::string format(const char *format,)//一个参数数量可变的函数
//    {
//        std::string ret;
//        va_list ap;//新特性
//        va_start(ap, format);
//        char *buf = (char *) malloc(1024 * 100);
//        if (buf != nullptr) {
//            vsnprintf(buf, 1024 * 100, format, ap);
//            ret = buf;
//            free(buf);
//        }
//        va_end(ap);
//        return ret;//返回字符串
//    };
//}
//
//#endif //DEMOFFMPEG_STRINGUTILS_H
