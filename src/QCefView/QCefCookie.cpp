#include "Include/QCefCookie.h"

#pragma region cef_headers
#include <include/cef_app.h>
#include <include/base/cef_bind.h>
#include <include/base/cef_bind_helpers.h>
#include <include/wrapper/cef_closure_task.h>
#include <include/cef_waitable_event.h>
#include <include/internal/cef_time.h>
#include <include/wrapper/cef_helpers.h>
#pragma endregion cef_headers


typedef std::vector<CefCookie> CookieVector;

class QCefCompletionCallback :
	public CefCompletionCallback
{
public:
	explicit QCefCompletionCallback(CefRefPtr<CefWaitableEvent> event)
		: event_(event) {
	}

	void OnComplete() override {
		CEF_REQUIRE_UI_THREAD();
		event_->Signal();
	}

private:
	CefRefPtr<CefWaitableEvent> event_;

	IMPLEMENT_REFCOUNTING(QCefCompletionCallback);
	DISALLOW_COPY_AND_ASSIGN(QCefCompletionCallback);
};


class QCefCookieVisitor :
	public CefCookieVisitor
{
public:
	QCefCookieVisitor(CookieVector* cookies,
		bool deleteCookies,
		const base::Closure& callback) : cookies_(cookies), delete_cookies_(deleteCookies), callback_(callback)
	{
		assert(cookies_);
		assert(!callback_.is_null());
	}

	~QCefCookieVisitor() {
		callback_.Run();
	}

	bool Visit(const CefCookie& cookie, int count, int total,
		bool& deleteCookie) {
		CEF_REQUIRE_UI_THREAD();
		cookies_->push_back(cookie);
		if (delete_cookies_)
			deleteCookie = true;
		return true;
	}

private:
	CookieVector* cookies_;
	bool delete_cookies_;
	base::Closure callback_;
	IMPLEMENT_REFCOUNTING(QCefCookieVisitor);
};

namespace detail {

	void VisitAllCookies(CefRefPtr<CefCookieManager> manager,
		CookieVector& cookies,
		bool deleteCookies,
		const base::Closure& callback)
	{
		manager->VisitAllCookies(
			new QCefCookieVisitor(&cookies, deleteCookies, callback));
	}

	void VisitAllCookies(CefRefPtr<CefCookieManager> manager,
		CookieVector& cookies,
		bool deleteCookies,
		CefRefPtr<CefWaitableEvent> event)
	{
		VisitAllCookies(manager, cookies, deleteCookies,
			base::Bind(&CefWaitableEvent::Signal, event));
		event->Wait();
	}

	void VisitUrlCookies(CefRefPtr<CefCookieManager> manager,
		const CefString& url,
		bool includeHttpOnly,
		CookieVector& cookies,
		bool deleteCookies,
		const base::Closure& callback)
	{
		manager->VisitUrlCookies(
			url, includeHttpOnly,
			new QCefCookieVisitor(&cookies, deleteCookies, callback));
	}
	
	void VisitUrlCookies(CefRefPtr<CefCookieManager> manager,
		const CefString& url,
		bool includeHttpOnly,
		CookieVector& cookies,
		bool deleteCookies,
		CefRefPtr<CefWaitableEvent> event)
	{
		VisitUrlCookies(manager, url, includeHttpOnly, cookies, deleteCookies,
			base::Bind(&CefWaitableEvent::Signal, event));
		event->Wait();
	}


	void GetAllCookies(CookieVector & cookies)
	{
		CefRefPtr<CefWaitableEvent> event =
			CefWaitableEvent::CreateWaitableEvent(true, false);

		CefRefPtr<CefCookieManager> manager =
			CefCookieManager::GetGlobalManager(new QCefCompletionCallback(event));
		event->Wait();
		VisitAllCookies(manager, cookies, false, event);
	}

	void GetCookieFromUrl(CookieVector & cookies, const CefString & url)
	{
		CefRefPtr<CefWaitableEvent> event =
			CefWaitableEvent::CreateWaitableEvent(true, false);

		CefRefPtr<CefCookieManager> manager =
			CefCookieManager::GetGlobalManager(new QCefCompletionCallback(event));
		event->Wait();
		VisitUrlCookies(manager, url, false, cookies, false, event);
	}
}


QCefCookie::QCefCookie()
	: QObject(nullptr)
{
}


QCefCookie::~QCefCookie()
{
}

QCefCookie & QCefCookie::getInstance()
{
	static QCefCookie s_instance;
	return s_instance;
}

QString QCefCookie::GetAllCookies()
{
	QString cookie_str;
	CookieVector cookies;
	detail::GetAllCookies(cookies);
	for (auto &it : cookies) {
		if (it.name.str && it.value.str) {
			QString cookie;
			cookie.append(QString::fromStdWString(it.name.str));
			cookie.append("=");
			cookie.append(QString::fromStdWString(it.value.str));
			cookie.append(";");
			cookie_str += cookie;
		}
	}

	cookie_str.remove(cookie_str.size() - 1);

	return cookie_str;
}

QString QCefCookie::GetCookieFromUrl(const QString & url)
{
	QString cookie_str;
	CookieVector cookies;
	detail::GetCookieFromUrl(cookies, url.toStdString());
	for (auto &it : cookies) {
		if (it.name.str && it.value.str) {
			QString cookie;
			cookie.append(QString::fromStdWString(it.name.str));
			cookie.append("=");
			cookie.append(QString::fromStdWString(it.value.str));
			cookie.append(";");
			cookie_str += cookie;
		}
	}

	cookie_str.remove(cookie_str.size() - 1);

	return cookie_str;
}

void QCefCookie::GetAllCookies(CookieContainer & cookies)
{
	CefRefPtr<CefWaitableEvent> event =
		CefWaitableEvent::CreateWaitableEvent(true, false);

	CefRefPtr<CefCookieManager> manager =
		CefCookieManager::GetGlobalManager(new QCefCompletionCallback(event));
	event->Wait();

	CookieVector cookie_vec;
	detail::VisitAllCookies(manager, cookie_vec, false, event);

	for (auto &it : cookie_vec) {
		QString name = QString::fromStdWString(it.name.str);
		QString value = QString::fromStdWString(it.value.str);
		if(!name.isEmpty() && !value.isEmpty()) {
			cookies[name] = value;
		}
	}
}

void QCefCookie::GetCookieFromUrl(CookieContainer & cookies, const QString & url)
{
	CefRefPtr<CefWaitableEvent> event =
		CefWaitableEvent::CreateWaitableEvent(true, false);

	CefRefPtr<CefCookieManager> manager =
		CefCookieManager::GetGlobalManager(new QCefCompletionCallback(event));
	event->Wait();

	CookieVector cookies_vec;
	detail::VisitUrlCookies(manager, CefString(url.toStdString()), false, cookies_vec, false, event);
}

bool QCefCookie::SetCookie(const QString &url,
	const QString &key,
	const QString &value,
	time_t expires)
{
	std::wstring domain = url.toStdWString();
	std::transform(domain.begin(), domain.end(), domain.begin(), tolower);

	std::size_t http_pos, https_pos;
	http_pos = domain.find(L"http://");
	https_pos = domain.find(L"https://");
	if (std::wstring::npos == http_pos &&
		std::wstring::npos == https_pos) {
		return false;
	}

	domain = domain.substr(domain.find(L"."), domain.size());
	domain = domain.substr(0, domain.find(L"/"));

	CefRefPtr<CefSetCookieCallback> callback = NULL;
	CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager(nullptr);
	CefCookie cookie;
	CefString(&cookie.name).FromWString(key.toStdWString());
	CefString(&cookie.value).FromWString(value.toStdWString());
	CefString(&cookie.domain).FromWString(domain.c_str());//.magpcss.org
	CefString(&cookie.path).FromASCII("/");

	cookie.has_expires = true;
	if (!cef_time_from_timet(expires, &cookie.expires)) {
		cookie.expires.year = 2099;
		cookie.expires.month = 3;
		cookie.expires.day_of_week = 5;
		cookie.expires.day_of_month = 20;
	}

	return CefPostTask(TID_IO, base::Bind(base::IgnoreResult(&CefCookieManager::SetCookie), manager.get(), CefString(url.toStdString()), cookie, callback));
}

bool QCefCookie::DeleteCookie(const QString &url,
	const QString &cookie_name)
{
	CefRefPtr<CefDeleteCookiesCallback> callback = NULL;
	CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager(nullptr);
	return CefPostTask(TID_IO, base::Bind(base::IgnoreResult(&CefCookieManager::DeleteCookies), manager.get(), CefString(url.toStdString()), CefString(cookie_name.toStdString()), callback));
}

