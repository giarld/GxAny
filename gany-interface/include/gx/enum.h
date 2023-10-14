/*
 * Copyright (c) 2020-present GiarldXin(Gxin)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef GX_ENUM_H
#define GX_ENUM_H

#include "gglobal.h"

#include <type_traits>


#define DEF_ENUM_OPERATORS(NAME, BASE_TYPE) \
    inline constexpr NAME operator~(NAME a) { return (NAME)~(BASE_TYPE)a; } \
    inline constexpr NAME operator|(NAME a, NAME b) { return (NAME)((BASE_TYPE)a | (BASE_TYPE)b); } \
    inline constexpr NAME operator&(NAME a, NAME b) { return (NAME)((BASE_TYPE)a & (BASE_TYPE)b); } \
    inline constexpr NAME operator^(NAME a, NAME b) { return (NAME)((BASE_TYPE)a ^ (BASE_TYPE)b); } \
    inline NAME &operator|=(NAME &a, NAME b) { return (NAME&)((BASE_TYPE&)a |= (BASE_TYPE)b); }     \
    inline NAME &operator&=(NAME &a, NAME b) { return (NAME&)((BASE_TYPE&)a &= (BASE_TYPE)b); }     \
    inline NAME &operator^=(NAME &a, NAME b) { return (NAME&)((BASE_TYPE&)a ^= (BASE_TYPE)b); }     \
    inline constexpr bool operator==(NAME a, NAME b) { return ((BASE_TYPE)a) == ((BASE_TYPE)b); }   \
    inline constexpr bool operator<(NAME a, NAME b) { return ((BASE_TYPE)a) < ((BASE_TYPE)b); }     \
    inline constexpr bool operator<=(NAME a, NAME b) { return ((BASE_TYPE)a) <= ((BASE_TYPE)b); }     \
    inline constexpr bool operator>(NAME a, NAME b) { return ((BASE_TYPE)a) > ((BASE_TYPE)b); }     \
    inline constexpr bool operator>=(NAME a, NAME b) { return ((BASE_TYPE)a) >= ((BASE_TYPE)b); }     \
    inline constexpr bool operator!(NAME a) { return ((BASE_TYPE)a) == 0; }

#define ENUM_NONE(ENUM_V) !(ENUM_V)

#define ENUM_ANY(ENUM_V) !!(ENUM_V)


#define DEF_ENUM_FLAGS_1(NAME, BASE_TYPE, T0, V0) \
    enum class NAME : BASE_TYPE { T0 = V0 }; \
    constexpr int32_t Enum##NAME##Count = 1; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0 }; \
         return keys; \
    } \
    static inline const BASE_TYPE *Enum##NAME##Vals() \
    { \
         static BASE_TYPE vals[] = { V0 }; \
         return vals; \
    } \
    static inline const char *EnumToString(NAME e) { \
        for (int32_t i = 0; i < Enum##NAME##Count; i++) { if (Enum##NAME##Vals()[i] == (BASE_TYPE)e) { return Enum##NAME##Strs()[(size_t)i]; } }\
        return ""; \
    } \
    DEF_ENUM_OPERATORS(NAME, BASE_TYPE)

#define DEF_ENUM_FLAGS_2(NAME, BASE_TYPE, T0, V0, T1, V1) \
    enum class NAME : BASE_TYPE { T0 = V0, T1 = V1 }; \
    constexpr int32_t Enum##NAME##Count = 2; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1 }; \
         return keys; \
    } \
    static inline const BASE_TYPE *Enum##NAME##Vals() \
    { \
         static BASE_TYPE vals[] = { V0, V1 };\
         return vals; \
    } \
    static inline const char *EnumToString(NAME e) { \
        for (int32_t i = 0; i < Enum##NAME##Count; i++) { if (Enum##NAME##Vals()[i] == (BASE_TYPE)e) { return Enum##NAME##Strs()[(size_t)i]; } }\
        return ""; \
    } \
    DEF_ENUM_OPERATORS(NAME, BASE_TYPE)

#define DEF_ENUM_FLAGS_3(NAME, BASE_TYPE, T0, V0, T1, V1, T2, V2) \
    enum class NAME : BASE_TYPE { T0 = V0, T1 = V1, T2 = V2 }; \
    constexpr int32_t Enum##NAME##Count = 3; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2 }; \
         return keys; \
    } \
    static inline const BASE_TYPE *Enum##NAME##Vals() \
    { \
         static BASE_TYPE vals[] = { V0, V1, V2 }; \
         return vals; \
    } \
    static inline const char *EnumToString(NAME e) { \
        for (int32_t i = 0; i < Enum##NAME##Count; i++) { if (Enum##NAME##Vals()[i] == (BASE_TYPE)e) { return Enum##NAME##Strs()[(size_t)i]; } }\
        return ""; \
    } \
    DEF_ENUM_OPERATORS(NAME, BASE_TYPE)

#define DEF_ENUM_FLAGS_4(NAME, BASE_TYPE, T0, V0, T1, V1, T2, V2, T3, V3) \
    enum class NAME : BASE_TYPE { T0 = V0, T1 = V1, T2 = V2, T3 = V3 }; \
    constexpr int32_t Enum##NAME##Count = 4; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3 }; \
         return keys; \
    } \
    static inline const BASE_TYPE *Enum##NAME##Vals() \
    { \
         static BASE_TYPE vals[] = { V0, V1, V2, V3 }; \
         return vals; \
    } \
    static inline const char *EnumToString(NAME e) { \
        for (int32_t i = 0; i < Enum##NAME##Count; i++) { if (Enum##NAME##Vals()[i] == (BASE_TYPE)e) { return Enum##NAME##Strs()[(size_t)i]; } }\
        return ""; \
    } \
    DEF_ENUM_OPERATORS(NAME, BASE_TYPE)

#define DEF_ENUM_FLAGS_5(NAME, BASE_TYPE, T0, V0, T1, V1, T2, V2, T3, V3, T4, V4) \
    enum class NAME : BASE_TYPE { T0 = V0, T1 = V1, T2 = V2, T3 = V3, T4 = V4 }; \
    constexpr int32_t Enum##NAME##Count = 5; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4 }; \
         return keys; \
    } \
    static inline const BASE_TYPE *Enum##NAME##Vals() \
    { \
         static BASE_TYPE vals[] = { V0, V1, V2, V3, V4 }; \
         return vals; \
    } \
    static inline const char *EnumToString(NAME e) { \
        for (int32_t i = 0; i < Enum##NAME##Count; i++) { if (Enum##NAME##Vals()[i] == (BASE_TYPE)e) { return Enum##NAME##Strs()[(size_t)i]; } }\
        return ""; \
    } \
    DEF_ENUM_OPERATORS(NAME, BASE_TYPE)

#define DEF_ENUM_FLAGS_6(NAME, BASE_TYPE, T0, V0, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5) \
    enum class NAME : BASE_TYPE { T0 = V0, T1 = V1, T2 = V2, T3 = V3, T4 = V4, T5 = V5 }; \
    constexpr int32_t Enum##NAME##Count = 6; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5 }; \
         return keys; \
    } \
    static inline const BASE_TYPE *Enum##NAME##Vals() \
    { \
         static BASE_TYPE vals[] = { V0, V1, V2, V3, V4, V5 }; \
         return vals; \
    } \
    static inline const char *EnumToString(NAME e) { \
        for (int32_t i = 0; i < Enum##NAME##Count; i++) { if (Enum##NAME##Vals()[i] == (BASE_TYPE)e) { return Enum##NAME##Strs()[(size_t)i]; } }\
        return ""; \
    } \
    DEF_ENUM_OPERATORS(NAME, BASE_TYPE)

#define DEF_ENUM_FLAGS_7(NAME, BASE_TYPE, T0, V0, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6) \
    enum class NAME : BASE_TYPE { T0 = V0, T1 = V1, T2 = V2, T3 = V3, T4 = V4, T5 = V5, T6 = V6 }; \
    constexpr int32_t Enum##NAME##Count = 7; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6 }; \
         return keys; \
    } \
    static inline const BASE_TYPE *Enum##NAME##Vals() \
    { \
         static BASE_TYPE vals[] = { V0, V1, V2, V3, V4, V5, V6 }; \
         return vals; \
    } \
    static inline const char *EnumToString(NAME e) { \
        for (int32_t i = 0; i < Enum##NAME##Count; i++) { if (Enum##NAME##Vals()[i] == (BASE_TYPE)e) { return Enum##NAME##Strs()[(size_t)i]; } }\
        return ""; \
    } \
    DEF_ENUM_OPERATORS(NAME, BASE_TYPE)

#define DEF_ENUM_FLAGS_8(NAME, BASE_TYPE, T0, V0, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7) \
    enum class NAME : BASE_TYPE { T0 = V0, T1 = V1, T2 = V2, T3 = V3, T4 = V4, T5 = V5, T6 = V6, T7 = V7 }; \
    constexpr int32_t Enum##NAME##Count = 8; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7 }; \
         return keys; \
    } \
    static inline const BASE_TYPE *Enum##NAME##Vals() \
    { \
         static BASE_TYPE vals[] = { V0, V1, V2, V3, V4, V5, V6, V7 }; \
         return vals; \
    } \
    static inline const char *EnumToString(NAME e) { \
        for (int32_t i = 0; i < Enum##NAME##Count; i++) { if (Enum##NAME##Vals()[i] == (BASE_TYPE)e) { return Enum##NAME##Strs()[(size_t)i]; } }\
        return ""; \
    } \
    DEF_ENUM_OPERATORS(NAME, BASE_TYPE)

#define DEF_ENUM_FLAGS_9(NAME, BASE_TYPE, T0, V0, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8) \
    enum class NAME : BASE_TYPE { T0 = V0, T1 = V1, T2 = V2, T3 = V3, T4 = V4, T5 = V5, T6 = V6, T7 = V7, T8 = V8 }; \
    constexpr int32_t Enum##NAME##Count = 9; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8 }; \
         return keys; \
    } \
    static inline const BASE_TYPE *Enum##NAME##Vals() \
    { \
         static BASE_TYPE vals[] = { V0, V1, V2, V3, V4, V5, V6, V7, V8 }; \
         return vals; \
    } \
    static inline const char *EnumToString(NAME e) { \
        for (int32_t i = 0; i < Enum##NAME##Count; i++) { if (Enum##NAME##Vals()[i] == (BASE_TYPE)e) { return Enum##NAME##Strs()[(size_t)i]; } }\
        return ""; \
    } \
    DEF_ENUM_OPERATORS(NAME, BASE_TYPE)

#define DEF_ENUM_FLAGS_10(NAME, BASE_TYPE, T0, V0, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9) \
    enum class NAME : BASE_TYPE { T0 = V0, T1 = V1, T2 = V2, T3 = V3, T4 = V4, T5 = V5, T6 = V6, T7 = V7, T8 = V8, T9 = V9 }; \
    constexpr int32_t Enum##NAME##Count = 10; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9 }; \
         return keys; \
    } \
    static inline const BASE_TYPE *Enum##NAME##Vals() \
    { \
         static BASE_TYPE vals[] = { V0, V1, V2, V3, V4, V5, V6, V7, V8, V9 }; \
         return vals; \
    } \
    static inline const char *EnumToString(NAME e) { \
        for (int32_t i = 0; i < Enum##NAME##Count; i++) { if (Enum##NAME##Vals()[i] == (BASE_TYPE)e) { return Enum##NAME##Strs()[(size_t)i]; } }\
        return ""; \
    } \
    DEF_ENUM_OPERATORS(NAME, BASE_TYPE)


#define DEF_ENUM_1(NAME, BASE_VAL, T0) \
    enum class NAME { T0 = BASE_VAL }; \
    constexpr int32_t Enum##NAME##Count = 1; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_2(NAME, BASE_VAL, T0, T1) \
    enum class NAME { T0 = BASE_VAL, T1 }; \
    constexpr int32_t Enum##NAME##Count = 2; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_3(NAME, BASE_VAL, T0, T1, T2) \
    enum class NAME { T0 = BASE_VAL, T1, T2 }; \
    constexpr int32_t Enum##NAME##Count = 3; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_4(NAME, BASE_VAL, T0, T1, T2, T3) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3 }; \
    constexpr int32_t Enum##NAME##Count = 4; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_5(NAME, BASE_VAL, T0, T1, T2, T3, T4) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4 }; \
    constexpr int32_t Enum##NAME##Count = 5; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_6(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5 }; \
    constexpr int32_t Enum##NAME##Count = 6; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_7(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6 }; \
    constexpr int32_t Enum##NAME##Count = 7; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_8(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7 }; \
    constexpr int32_t Enum##NAME##Count = 8; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_9(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8 }; \
    constexpr int32_t Enum##NAME##Count = 9; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_10(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9 }; \
    constexpr int32_t Enum##NAME##Count = 10; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_11(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 }; \
    constexpr int32_t Enum##NAME##Count = 11; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_12(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11 }; \
    constexpr int32_t Enum##NAME##Count = 12; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10), GX_STRINGIZE(T11) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10, NAME::T11 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_13(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12 }; \
    constexpr int32_t Enum##NAME##Count = 13; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10), GX_STRINGIZE(T11), GX_STRINGIZE(T12) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10, NAME::T11, NAME::T12 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_14(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13 }; \
    constexpr int32_t Enum##NAME##Count = 14; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10), GX_STRINGIZE(T11), GX_STRINGIZE(T12), GX_STRINGIZE(T13) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10, NAME::T11, NAME::T12, NAME::T13 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_15(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14 }; \
    constexpr int32_t Enum##NAME##Count = 15; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10), GX_STRINGIZE(T11), GX_STRINGIZE(T12), GX_STRINGIZE(T13), GX_STRINGIZE(T14) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10, NAME::T11, NAME::T12, NAME::T13, NAME::T14 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_16(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15 }; \
    constexpr int32_t Enum##NAME##Count = 16; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10), GX_STRINGIZE(T11), GX_STRINGIZE(T12), GX_STRINGIZE(T13), GX_STRINGIZE(T14), GX_STRINGIZE(T15) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10, NAME::T11, NAME::T12, NAME::T13, NAME::T14, NAME::T15 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_17(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16 }; \
    constexpr int32_t Enum##NAME##Count = 17; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10), GX_STRINGIZE(T11), GX_STRINGIZE(T12), GX_STRINGIZE(T13), GX_STRINGIZE(T14), GX_STRINGIZE(T15), GX_STRINGIZE(T16) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10, NAME::T11, NAME::T12, NAME::T13, NAME::T14, NAME::T15, NAME::T16 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_18(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17 }; \
    constexpr int32_t Enum##NAME##Count = 18; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10), GX_STRINGIZE(T11), GX_STRINGIZE(T12), GX_STRINGIZE(T13), GX_STRINGIZE(T14), GX_STRINGIZE(T15), GX_STRINGIZE(T16), GX_STRINGIZE(T17) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10, NAME::T11, NAME::T12, NAME::T13, NAME::T14, NAME::T15, NAME::T16, NAME::T17 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_19(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18 }; \
    constexpr int32_t Enum##NAME##Count = 19; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10), GX_STRINGIZE(T11), GX_STRINGIZE(T12), GX_STRINGIZE(T13), GX_STRINGIZE(T14), GX_STRINGIZE(T15), GX_STRINGIZE(T16), GX_STRINGIZE(T17), GX_STRINGIZE(T18) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10, NAME::T11, NAME::T12, NAME::T13, NAME::T14, NAME::T15, NAME::T16, NAME::T17, NAME::T18 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_20(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19 }; \
    constexpr int32_t Enum##NAME##Count = 20; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10), GX_STRINGIZE(T11), GX_STRINGIZE(T12), GX_STRINGIZE(T13), GX_STRINGIZE(T14), GX_STRINGIZE(T15), GX_STRINGIZE(T16), GX_STRINGIZE(T17), GX_STRINGIZE(T18), GX_STRINGIZE(T19) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10, NAME::T11, NAME::T12, NAME::T13, NAME::T14, NAME::T15, NAME::T16, NAME::T17, NAME::T18, NAME::T19 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_21(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 }; \
    constexpr int32_t Enum##NAME##Count = 21; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10), GX_STRINGIZE(T11), GX_STRINGIZE(T12), GX_STRINGIZE(T13), GX_STRINGIZE(T14), GX_STRINGIZE(T15), GX_STRINGIZE(T16), GX_STRINGIZE(T17), GX_STRINGIZE(T18), GX_STRINGIZE(T19), GX_STRINGIZE(T20) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10, NAME::T11, NAME::T12, NAME::T13, NAME::T14, NAME::T15, NAME::T16, NAME::T17, NAME::T18, NAME::T19, NAME::T20 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_22(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21 }; \
    constexpr int32_t Enum##NAME##Count = 22; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10), GX_STRINGIZE(T11), GX_STRINGIZE(T12), GX_STRINGIZE(T13), GX_STRINGIZE(T14), GX_STRINGIZE(T15), GX_STRINGIZE(T16), GX_STRINGIZE(T17), GX_STRINGIZE(T18), GX_STRINGIZE(T19), GX_STRINGIZE(T20), GX_STRINGIZE(T21) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10, NAME::T11, NAME::T12, NAME::T13, NAME::T14, NAME::T15, NAME::T16, NAME::T17, NAME::T18, NAME::T19, NAME::T20, NAME::T21 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_23(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22 }; \
    constexpr int32_t Enum##NAME##Count = 23; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10), GX_STRINGIZE(T11), GX_STRINGIZE(T12), GX_STRINGIZE(T13), GX_STRINGIZE(T14), GX_STRINGIZE(T15), GX_STRINGIZE(T16), GX_STRINGIZE(T17), GX_STRINGIZE(T18), GX_STRINGIZE(T19), GX_STRINGIZE(T20), GX_STRINGIZE(T21), GX_STRINGIZE(T22) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10, NAME::T11, NAME::T12, NAME::T13, NAME::T14, NAME::T15, NAME::T16, NAME::T17, NAME::T18, NAME::T19, NAME::T20, NAME::T21, NAME::T22 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_24(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23 }; \
    constexpr int32_t Enum##NAME##Count = 24; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10), GX_STRINGIZE(T11), GX_STRINGIZE(T12), GX_STRINGIZE(T13), GX_STRINGIZE(T14), GX_STRINGIZE(T15), GX_STRINGIZE(T16), GX_STRINGIZE(T17), GX_STRINGIZE(T18), GX_STRINGIZE(T19), GX_STRINGIZE(T20), GX_STRINGIZE(T21), GX_STRINGIZE(T22), GX_STRINGIZE(T23) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10, NAME::T11, NAME::T12, NAME::T13, NAME::T14, NAME::T15, NAME::T16, NAME::T17, NAME::T18, NAME::T19, NAME::T20, NAME::T21, NAME::T22, NAME::T23 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_25(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24 }; \
    constexpr int32_t Enum##NAME##Count = 25; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10), GX_STRINGIZE(T11), GX_STRINGIZE(T12), GX_STRINGIZE(T13), GX_STRINGIZE(T14), GX_STRINGIZE(T15), GX_STRINGIZE(T16), GX_STRINGIZE(T17), GX_STRINGIZE(T18), GX_STRINGIZE(T19), GX_STRINGIZE(T20), GX_STRINGIZE(T21), GX_STRINGIZE(T22), GX_STRINGIZE(T23), GX_STRINGIZE(T24) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10, NAME::T11, NAME::T12, NAME::T13, NAME::T14, NAME::T15, NAME::T16, NAME::T17, NAME::T18, NAME::T19, NAME::T20, NAME::T21, NAME::T22, NAME::T23, NAME::T24 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_26(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25 }; \
    constexpr int32_t Enum##NAME##Count = 26; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10), GX_STRINGIZE(T11), GX_STRINGIZE(T12), GX_STRINGIZE(T13), GX_STRINGIZE(T14), GX_STRINGIZE(T15), GX_STRINGIZE(T16), GX_STRINGIZE(T17), GX_STRINGIZE(T18), GX_STRINGIZE(T19), GX_STRINGIZE(T20), GX_STRINGIZE(T21), GX_STRINGIZE(T22), GX_STRINGIZE(T23), GX_STRINGIZE(T24), GX_STRINGIZE(T25) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10, NAME::T11, NAME::T12, NAME::T13, NAME::T14, NAME::T15, NAME::T16, NAME::T17, NAME::T18, NAME::T19, NAME::T20, NAME::T21, NAME::T22, NAME::T23, NAME::T24, NAME::T25 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_27(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26 }; \
    constexpr int32_t Enum##NAME##Count = 27; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10), GX_STRINGIZE(T11), GX_STRINGIZE(T12), GX_STRINGIZE(T13), GX_STRINGIZE(T14), GX_STRINGIZE(T15), GX_STRINGIZE(T16), GX_STRINGIZE(T17), GX_STRINGIZE(T18), GX_STRINGIZE(T19), GX_STRINGIZE(T20), GX_STRINGIZE(T21), GX_STRINGIZE(T22), GX_STRINGIZE(T23), GX_STRINGIZE(T24), GX_STRINGIZE(T25), GX_STRINGIZE(T26) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10, NAME::T11, NAME::T12, NAME::T13, NAME::T14, NAME::T15, NAME::T16, NAME::T17, NAME::T18, NAME::T19, NAME::T20, NAME::T21, NAME::T22, NAME::T23, NAME::T24, NAME::T25, NAME::T26 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_28(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27 }; \
    constexpr int32_t Enum##NAME##Count = 28; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10), GX_STRINGIZE(T11), GX_STRINGIZE(T12), GX_STRINGIZE(T13), GX_STRINGIZE(T14), GX_STRINGIZE(T15), GX_STRINGIZE(T16), GX_STRINGIZE(T17), GX_STRINGIZE(T18), GX_STRINGIZE(T19), GX_STRINGIZE(T20), GX_STRINGIZE(T21), GX_STRINGIZE(T22), GX_STRINGIZE(T23), GX_STRINGIZE(T24), GX_STRINGIZE(T25), GX_STRINGIZE(T26), GX_STRINGIZE(T27) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10, NAME::T11, NAME::T12, NAME::T13, NAME::T14, NAME::T15, NAME::T16, NAME::T17, NAME::T18, NAME::T19, NAME::T20, NAME::T21, NAME::T22, NAME::T23, NAME::T24, NAME::T25, NAME::T26, NAME::T27 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_29(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28 }; \
    constexpr int32_t Enum##NAME##Count = 29; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10), GX_STRINGIZE(T11), GX_STRINGIZE(T12), GX_STRINGIZE(T13), GX_STRINGIZE(T14), GX_STRINGIZE(T15), GX_STRINGIZE(T16), GX_STRINGIZE(T17), GX_STRINGIZE(T18), GX_STRINGIZE(T19), GX_STRINGIZE(T20), GX_STRINGIZE(T21), GX_STRINGIZE(T22), GX_STRINGIZE(T23), GX_STRINGIZE(T24), GX_STRINGIZE(T25), GX_STRINGIZE(T26), GX_STRINGIZE(T27), GX_STRINGIZE(T28) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10, NAME::T11, NAME::T12, NAME::T13, NAME::T14, NAME::T15, NAME::T16, NAME::T17, NAME::T18, NAME::T19, NAME::T20, NAME::T21, NAME::T22, NAME::T23, NAME::T24, NAME::T25, NAME::T26, NAME::T27, NAME::T28 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_30(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29 }; \
    constexpr int32_t Enum##NAME##Count = 30; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10), GX_STRINGIZE(T11), GX_STRINGIZE(T12), GX_STRINGIZE(T13), GX_STRINGIZE(T14), GX_STRINGIZE(T15), GX_STRINGIZE(T16), GX_STRINGIZE(T17), GX_STRINGIZE(T18), GX_STRINGIZE(T19), GX_STRINGIZE(T20), GX_STRINGIZE(T21), GX_STRINGIZE(T22), GX_STRINGIZE(T23), GX_STRINGIZE(T24), GX_STRINGIZE(T25), GX_STRINGIZE(T26), GX_STRINGIZE(T27), GX_STRINGIZE(T28), GX_STRINGIZE(T29) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10, NAME::T11, NAME::T12, NAME::T13, NAME::T14, NAME::T15, NAME::T16, NAME::T17, NAME::T18, NAME::T19, NAME::T20, NAME::T21, NAME::T22, NAME::T23, NAME::T24, NAME::T25, NAME::T26, NAME::T27, NAME::T28, NAME::T29 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_31(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 }; \
    constexpr int32_t Enum##NAME##Count = 31; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10), GX_STRINGIZE(T11), GX_STRINGIZE(T12), GX_STRINGIZE(T13), GX_STRINGIZE(T14), GX_STRINGIZE(T15), GX_STRINGIZE(T16), GX_STRINGIZE(T17), GX_STRINGIZE(T18), GX_STRINGIZE(T19), GX_STRINGIZE(T20), GX_STRINGIZE(T21), GX_STRINGIZE(T22), GX_STRINGIZE(T23), GX_STRINGIZE(T24), GX_STRINGIZE(T25), GX_STRINGIZE(T26), GX_STRINGIZE(T27), GX_STRINGIZE(T28), GX_STRINGIZE(T29), GX_STRINGIZE(T30) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10, NAME::T11, NAME::T12, NAME::T13, NAME::T14, NAME::T15, NAME::T16, NAME::T17, NAME::T18, NAME::T19, NAME::T20, NAME::T21, NAME::T22, NAME::T23, NAME::T24, NAME::T25, NAME::T26, NAME::T27, NAME::T28, NAME::T29, NAME::T30 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#define DEF_ENUM_32(NAME, BASE_VAL, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31) \
    enum class NAME { T0 = BASE_VAL, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31 }; \
    constexpr int32_t Enum##NAME##Count = 32; \
    static inline const char **Enum##NAME##Strs() \
    { \
        static const char *str[] = { GX_STRINGIZE(T0), GX_STRINGIZE(T1), GX_STRINGIZE(T2), GX_STRINGIZE(T3), GX_STRINGIZE(T4), GX_STRINGIZE(T5), GX_STRINGIZE(T6), GX_STRINGIZE(T7), GX_STRINGIZE(T8), GX_STRINGIZE(T9), GX_STRINGIZE(T10), GX_STRINGIZE(T11), GX_STRINGIZE(T12), GX_STRINGIZE(T13), GX_STRINGIZE(T14), GX_STRINGIZE(T15), GX_STRINGIZE(T16), GX_STRINGIZE(T17), GX_STRINGIZE(T18), GX_STRINGIZE(T19), GX_STRINGIZE(T20), GX_STRINGIZE(T21), GX_STRINGIZE(T22), GX_STRINGIZE(T23), GX_STRINGIZE(T24), GX_STRINGIZE(T25), GX_STRINGIZE(T26), GX_STRINGIZE(T27), GX_STRINGIZE(T28), GX_STRINGIZE(T29), GX_STRINGIZE(T30), GX_STRINGIZE(T31) }; \
        return str; \
    } \
    static inline const NAME *Enum##NAME##Keys() \
    { \
         static NAME keys[] = { NAME::T0, NAME::T1, NAME::T2, NAME::T3, NAME::T4, NAME::T5, NAME::T6, NAME::T7, NAME::T8, NAME::T9, NAME::T10, NAME::T11, NAME::T12, NAME::T13, NAME::T14, NAME::T15, NAME::T16, NAME::T17, NAME::T18, NAME::T19, NAME::T20, NAME::T21, NAME::T22, NAME::T23, NAME::T24, NAME::T25, NAME::T26, NAME::T27, NAME::T28, NAME::T29, NAME::T30, NAME::T31 }; \
         return keys; \
    } \
    static inline const char *EnumToString(NAME e) { \
        return Enum##NAME##Strs()[(size_t)((int32_t)e - BASE_VAL)]; \
    } \
    DEF_ENUM_OPERATORS(NAME, int32_t)

#endif //GX_ENUM_H
