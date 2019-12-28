// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#ifndef _ERROR_H_
#define _ERROR_H_

namespace Error {
enum IOError { loading_failure, saving_failure };
enum ASMError { syntax_error, duplicate_define, undefine_symbol };
}  // namespace Error

#endif
