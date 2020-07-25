#ifndef CONFIGSETTINGDIALOG_H
#define CONFIGSETTINGDIALOG_H

#include <QDialog>
#include <QtDebug>
#include "globaldatadefinitions.h"

namespace Ui {
class ConfigSettingDialog;
}

class ConfigSettingDialog : public QDialog
{
		Q_OBJECT
	public:
		explicit ConfigSettingDialog(QWidget *parent = nullptr);
		~ConfigSettingDialog();

	signals:
		void settingsUpdate(const ApplicationSettings &settings);

	private slots:
		void on_settingsButtons_accepted();
		void on_settingsButtons_rejected();

	private:
		void initGui();
		void loadSettings();


		Ui::ConfigSettingDialog *ui;
		QString settingsFile;
		ApplicationSettings appSettings;


};

#endif // CONFIGSETTINGDIALOG_H
