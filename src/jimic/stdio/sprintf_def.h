
#ifndef _JIMIC_STDIO_SPRINTF_DEF_H_
#define _JIMIC_STDIO_SPRINTF_DEF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

//
// Printf() 输出格式控制
// Reference: http://bbs.csdn.net/topics/330107715
//

#ifndef _JIMIC_STDIO_SPRINTF_FORMAT_DEF_
#define _JIMIC_STDIO_SPRINTF_FORMAT_DEF_

/* format sign */
#define FMT_DEFAULT_FLAG        0x00000000      // default flag
#define FMT_SPACE_FLAG          0x00010000      // ' ', space flag
#define FMT_PLUS_FLAG           0x00020000      // '+', plus flag
#define FMT_SHARP_FLAG          0x00040000      // '#', number flag
#define FMT_FIELDWIDTH_FLAG     0x00080000      // '*', asterisk flag
#define FMT_CURRENCY_FLAG       0x00100000      // ',', comma flag

#define FMT_SIGN_MASK           0x00000002      // sign mask
#define FMT_SIGN_MASK_BIT       2               // sign move bit

// 句号: period

/* format align mode */
#define FMT_ALIGN_RIGHT         0x00200000      // align to right
#define FMT_ALIGN_LEFT          0x00400000      // align to left
#define FMT_ALIGN_CENTER        0x00800000      // align to center
#define FMT_ALIGN_MINUS         FMT_ALIGN_LEFT  // '-', minus sign
#define FMT_ALIGN_DEFAULT       FMT_ALIGN_RIGHT
                                                // default is FMT_ALIGN_RIGHT, '+'

/* format fill mode */
#define FMT_FILL_SPACE          ' '             // fill space: ' '
#define FMT_FILL_ZERO           '0'             // fill zero:  '0'
#define FMT_FILL_DEFAULT        FMT_FILL_SPACE
                                                // default is FMT_FILL_SPACE, ' '

#define FMT_DEFAULT_STATUS      (FMT_DEFAULT_FLAG | FMT_ALIGN_DEFAULT | FMT_FILL_DEFAULT)

#endif  /* !_JIMIC_STDIO_SPRINTF_FORMAT_DEF_ */

//
// ASCII码对照表
// Reference: http://ascii.911cha.com/
//

#ifndef FMT_DEFAULT_DOUBLE_PRECISION
#define FMT_DEFAULT_DOUBLE_PRECISION    6
#endif

#ifndef FMT_MAX_DOUBLE_PRECISION
#define FMT_MAX_DOUBLE_PRECISION        17
#endif

#endif  /* !_JIMIC_STDIO_SPRINTF_DEF_H_ */
