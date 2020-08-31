#ifndef CUSTOMCEFVIEW_H
#define CUSTOMCEFVIEW_H

#include <include/QCefView.h>
#include <include/QCefCookie.h>

class CustomCefView 
	: public QCefView
{
	Q_OBJECT

public:
	CustomCefView(const QString& url, QWidget *parent);
	~CustomCefView();

	void goBack();
	void goForward();
	void goUrl(const QString &url);

	QString getCookie();

protected:

	virtual void onLoadEnd(int browserId, int frameId, 
		const QString& frameUrl, int httpStatusCode);

	virtual void onQCefUrlRequest(const QString& url) override;

	virtual void onQCefQueryRequest(const QCefQuery& query) override;

	virtual void onInvokeMethodNotify(int browserId, int frameId, 
		const QString& method, 
		const QVariantList& arguments) override;

private:
	
};

#endif // CUSTOMCEFVIEW_H
