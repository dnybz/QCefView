#ifndef QCEFCOOKIE_H
#define QCEFCOOKIE_H
#pragma once

#pragma region qt_headers
#include <QtCore/qglobal.h>
#include <QObject>
#include <QString>
#include <QMap>
#pragma endregion qt_headers

#ifdef QCEFVIEW_LIB
#define QCEFVIEW_EXPORT Q_DECL_EXPORT
#else
#define QCEFVIEW_EXPORT Q_DECL_IMPORT
#if _WIN32
#pragma comment(lib, "QCefView.lib")
#endif
#endif

typedef QMap<QString, QString> CookieContainer;

class QCEFVIEW_EXPORT QCefCookie : public QObject
{
	Q_OBJECT
public:
	QCefCookie();
	~QCefCookie();

	static QCefCookie& getInstance();

	// Get the cookies string.
	QString GetAllCookies();

	// Get the cookies string form url.
	QString GetCookieFromUrl(const QString& url);

	// Get the cookies.
	void GetAllCookies(CookieContainer &cookies);

	// Get the cookies form url.
	void GetCookieFromUrl(CookieContainer &cookies, const QString& url);

	// Set the cookie.
	bool SetCookie(const QString &url,
		const QString &key,
		const QString &value,
		time_t expires);

	// Delete the cookie
	bool DeleteCookie(const QString &url,
		const QString &cookie_name);
};


#endif // QCEFCOOKIE_H