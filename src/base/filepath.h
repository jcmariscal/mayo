/****************************************************************************
** Copyright (c) 2021, Fougue Ltd. <http://www.fougue.pro>
** All rights reserved.
** See license at https://github.com/fougue/mayo/blob/master/LICENSE.txt
****************************************************************************/

#pragma once

// Workaround bug https://bugreports.qt.io/browse/QTBUG-73263 which affects GCC builds
#ifndef Q_MOC_RUN
#  include <filesystem>
#endif

namespace Mayo {

using FilePath = std::filesystem::path;

} // namespace Mayo


#include <QtCore/QString>
#include <QtCore/QFileInfo>
#include <TCollection_ExtendedString.hxx>
#include <type_traits>

namespace Mayo {

// Returns a FilePath object constructed from input
inline FilePath filepathFrom(const QByteArray& bytes) { return std::filesystem::u8path(bytes.constData()); }
inline FilePath filepathFrom(const QString& str) { return reinterpret_cast<const char16_t*>(str.utf16()); }
inline FilePath filepathFrom(const QFileInfo& fi) { return filepathFrom(fi.filePath()); }

// Returns the object converted to 'T' from filepath 'fp'
template<typename T> T filepathTo(const FilePath& fp)
{
    if constexpr(std::is_same_v<T, QByteArray>) {
        return QByteArray::fromStdString(fp.u8string());
    }
    if constexpr(std::is_same_v<T, QString>) {
        return QString::fromStdString(fp.u8string());
    }
    else if constexpr(std::is_same_v<T, QFileInfo>) {
        return QFileInfo(filepathTo<QString>(fp));
    }
    else if constexpr(std::is_same_v<T, TCollection_ExtendedString>) {
        if constexpr(std::is_same_v<FilePath::value_type, char>) {
            constexpr bool multiByte = true;
            return TCollection_ExtendedString(fp.c_str(), multiByte);
        }
        else if constexpr(std::is_same_v<FilePath::value_type, wchar_t>) {
            return TCollection_ExtendedString(fp.c_str());
        }
    }
}

// Exception-safe version of std::filesystem::equivalent()
inline bool filepathExists(const FilePath& fp) {
    try {
        return std::filesystem::exists(fp);
    } catch (...) { // fs::exists() might throw on non-existing files
        return false;
    }
}

// Exception-safe version of std::filesystem::equivalent()
inline bool filepathEquivalent(const FilePath& lhs, const FilePath& rhs) {
    try {
        return std::filesystem::equivalent(lhs, rhs);
    } catch (...) { // fs::equivalent() might throw on non-existing files
        return false;
    }
}

// Exception-safe version of std::filesystem::is_regular_file()
inline bool filepathIsRegularFile(const FilePath& fp) {
    try {
        return std::filesystem::is_regular_file(fp);
    } catch (...) {
        return false;
    }
}

} // namespace Mayo
