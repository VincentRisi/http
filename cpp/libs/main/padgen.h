#ifndef _PADGEN_H
#define _PADGEN_H

// This file is used to ensure 8 byte marshalling of data
// it is generated by padgen.exe and should not be hand maintained
//
//   template <class T>
//   struct NTBU
//   {
//       char x;
//       T    y;
//       char z;
//   };
//   struct STRUCT
//   {
//       double  len;
//       char    x;
//   };
//   struct STRUCT2
//   {
//     struct {int a; char b;} c;
//   };
//

/**
 * char sizeof=1 offset x=0 offset y=1 offset z=2
 */
#define IDL2_CHAR_PAD(no, size) char _f_##no[size]

/**
 * int8 sizeof=1 offset x=0 offset y=1 offset z=2
 */
#define IDL2_INT8_PAD(no) char _f_##no[7]

/**
 * int16 sizeof=2 offset x=0 offset y=2 offset z=4
 */
#define IDL2_INT16_PAD(no) char _f_##no[6]

/**
 * int32 sizeof=4 offset x=0 offset y=4 offset z=8
 */
#define IDL2_INT32_PAD(no) char _f_##no[4]

/**
 * int64 sizeof=8 offset x=0 offset y=8 offset z=16
 */
#define IDL2_INT64_PAD(no)

/**
 * double sizeof=8 offset x=0 offset y=4 offset z=12
 */
#define IDL2_DOUBLE_PAD(no)

/**
 * STRUCT sizeof=16 offset x=0 offset y=4 offset z=20
 */
#define IDL2_STRUCT_PAD(no)

/**
 * STRUCT2 sizeof=8 offset x=0 offset y=4 offset z=12
 */

#endif
