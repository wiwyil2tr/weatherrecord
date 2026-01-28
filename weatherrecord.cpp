#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QDateTime>
#include <QMessageBox>
#include <QVector>
#include <QString>
#include <QScrollArea>
#include <QGroupBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

// Weather record structure using Qt types (automatic memory management)
struct Weather {
    int temperature;    // Celsius
    int humidity;       // Percentage
    QString phenomenon; // e.g., "sunny", "rainy"
    QString date;       // YYYY-MM-DD format
    QString time;       // "morning" or "afternoon"
};

// Weather book container using Qt containers
class WeatherBook {
private:
    QVector<Weather> records;
    int capacity;

public:
    explicit WeatherBook(int cap = 10) : capacity(cap) {}
    
    bool addRecord(const Weather& record) {
        if (records.size() >= capacity) return false;
        records.append(record);
        return true;
    }
    
    QVector<Weather> queryRecords(const QString& date, const QString& time) const {
        QVector<Weather> results;
        for (const auto& record : records) {
            if (record.date == date && record.time == time) {
                results.append(record);
            }
        }
        return results;
    }
    
    int getSize() const { return records.size(); }
    int getCapacity() const { return capacity; }
};

// Main application window
class WeatherRecordBook : public QMainWindow {
    Q_OBJECT

private:
    WeatherBook book;
    QLineEdit *temperatureEdit;
    QLineEdit *humidityEdit;
    QLineEdit *phenomenonEdit;
    QLineEdit *dateEdit;
    QLineEdit *timeEdit;
    QTextEdit *outputView;
    QPushButton *addButton;
    QPushButton *queryButton;
    QPushButton *clearButton;

    // Determine travel suitability based on weather conditions
    QString judgeTravelSuitability(const Weather& record) const {
        const int LOW_TEMPERATURE = -10;
        const int HIGH_TEMPERATURE = 35;
        const int LOW_HUMIDITY = 30;
        const int HIGH_HUMIDITY = 80;

        // Check extreme conditions first
        if (record.temperature < LOW_TEMPERATURE || record.temperature > HIGH_TEMPERATURE) {
            return "Not suitable (extreme temperature)";
        }
        if (record.humidity < LOW_HUMIDITY || record.humidity > HIGH_HUMIDITY) {
            return "Not comfortable (humidity issues)";
        }

        // Check weather phenomenon
        if (record.phenomenon.toLower() == "sunny") return "Excellent - ideal for travel";
        if (record.phenomenon.toLower() == "cloudy") return "Good - acceptable conditions";
        if (record.phenomenon.toLower() == "rainy") return "Fair - bring rain gear";
        if (record.phenomenon.toLower() == "snowy") return "Poor - slippery conditions";
        if (record.phenomenon.toLower() == "stormy") return "Dangerous - avoid travel";
        
        return "Unknown conditions";
    }

    // Format weather record for display
    QString formatWeatherRecord(const Weather& record) const {
        QString suitability = judgeTravelSuitability(record);
        return QString(
            "🌡️ Temperature: %1°C\n"
            "💧 Humidity: %2%\n"
            "🌤️ Phenomenon: %3\n"
            "📅 Date: %4\n"
            "⏰ Time: %5\n"
            "✅ Travel Suitability: %6\n"
        ).arg(record.temperature)
         .arg(record.humidity)
         .arg(record.phenomenon)
         .arg(record.date)
         .arg(record.time)
         .arg(suitability);
    }

    // Get current date in YYYY-MM-DD format
    QString getCurrentDate() const {
        return QDateTime::currentDateTime().toString("yyyy-MM-dd");
    }

    // Get current time period (morning/afternoon)
    QString getCurrentTimePeriod() const {
        int hour = QDateTime::currentDateTime().time().hour();
        return (hour < 12) ? "morning" : "afternoon";
    }

    // Validate input fields before adding record
    bool validateInputs(QString& errorMessage) const {
        bool ok;
        
        // Temperature validation
        int temp = temperatureEdit->text().toInt(&ok);
        if (!ok || temp < -273 || temp > 100) {
            errorMessage = "Invalid temperature! Must be between -273°C and 100°C.";
            return false;
        }
        
        // Humidity validation
        int humidity = humidityEdit->text().toInt(&ok);
        if (!ok || humidity < 0 || humidity > 100) {
            errorMessage = "Invalid humidity! Must be between 0% and 100%.";
            return false;
        }
        
        // Non-empty fields validation
        if (phenomenonEdit->text().trimmed().isEmpty()) {
            errorMessage = "Phenomenon cannot be empty!";
            return false;
        }
        if (dateEdit->text().trimmed().isEmpty()) {
            errorMessage = "Date cannot be empty!";
            return false;
        }
        if (timeEdit->text().trimmed().isEmpty()) {
            errorMessage = "Time cannot be empty!";
            return false;
        }
        
        // Time period validation
        QString timeStr = timeEdit->text().trimmed().toLower();
        if (timeStr != "morning" && timeStr != "afternoon") {
            errorMessage = "Time must be either 'morning' or 'afternoon'.";
            return false;
        }
        
        return true;
    }

private slots:
    void onAddRecord() {
        QString error;
        if (!validateInputs(error)) {
            outputView->setPlainText("❌ Error: " + error);
            return;
        }

        Weather record;
        record.temperature = temperatureEdit->text().toInt();
        record.humidity = humidityEdit->text().toInt();
        record.phenomenon = phenomenonEdit->text().trimmed();
        record.date = dateEdit->text().trimmed();
        record.time = timeEdit->text().trimmed().toLower();

        if (book.addRecord(record)) {
            outputView->setPlainText(QString("✅ Weather record added successfully!\n\n%1")
                                     .arg(formatWeatherRecord(record)));
        } else {
            outputView->setPlainText(QString("❌ Error: Weather book is full!\n"
                                             "Maximum capacity: %1 records\n"
                                             "Current records: %2")
                                     .arg(book.getCapacity())
                                     .arg(book.getSize()));
        }
    }

    void onQueryRecords() {
        QString date = dateEdit->text().trimmed();
        QString time = timeEdit->text().trimmed().toLower();
        
        if (date.isEmpty() || (time != "morning" && time != "afternoon")) {
            outputView->setPlainText("❌ Error: Please enter valid date and time (morning/afternoon).");
            return;
        }

        QVector<Weather> results = book.queryRecords(date, time);
        
        if (results.isEmpty()) {
            outputView->setPlainText(QString("🔍 No records found for:\nDate: %1\nTime: %2")
                                     .arg(date).arg(time));
            return;
        }

        QString output = QString("📊 Found %1 record(s) for %2 (%3):\n\n")
                         .arg(results.size()).arg(date).arg(time);
        
        for (int i = 0; i < results.size(); ++i) {
            output += QString("Record #%1:\n%2\n").arg(i + 1).arg(formatWeatherRecord(results[i]));
        }
        
        outputView->setPlainText(output);
    }

    void onClearFields() {
        temperatureEdit->clear();
        humidityEdit->clear();
        phenomenonEdit->clear();
        dateEdit->setText(getCurrentDate());
        timeEdit->setText(getCurrentTimePeriod());
        outputView->clear();
    }

public:
    explicit WeatherRecordBook(QWidget *parent = nullptr) : QMainWindow(parent), book(10) {
        setWindowTitle("🌦️ Weather Record Book");
        resize(800, 600);
        
        // Central widget and main layout
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        
        // Input group box
        QGroupBox *inputGroup = new QGroupBox("🌤️ Weather Data Entry", this);
        QFormLayout *formLayout = new QFormLayout(inputGroup);
        
        temperatureEdit = new QLineEdit(this);
        temperatureEdit->setPlaceholderText("e.g., 25");
        formLayout->addRow("Temperature (°C):", temperatureEdit);
        
        humidityEdit = new QLineEdit(this);
        humidityEdit->setPlaceholderText("e.g., 65");
        formLayout->addRow("Humidity (%):", humidityEdit);
        
        phenomenonEdit = new QLineEdit(this);
        phenomenonEdit->setPlaceholderText("e.g., sunny, rainy, cloudy");
        formLayout->addRow("Phenomenon:", phenomenonEdit);
        
        dateEdit = new QLineEdit(this);
        dateEdit->setText(getCurrentDate());
        dateEdit->setPlaceholderText("YYYY-MM-DD");
        formLayout->addRow("Date:", dateEdit);
        
        timeEdit = new QLineEdit(this);
        timeEdit->setText(getCurrentTimePeriod());
        timeEdit->setPlaceholderText("morning or afternoon");
        formLayout->addRow("Time of Day:", timeEdit);
        
        // Button panel
        QWidget *buttonPanel = new QWidget(this);
        QHBoxLayout *buttonLayout = new QHBoxLayout(buttonPanel);
        
        addButton = new QPushButton("✅ Add Record", this);
        queryButton = new QPushButton("🔍 Query Records", this);
        clearButton = new QPushButton("🧹 Clear Fields", this);
        
        buttonLayout->addWidget(addButton);
        buttonLayout->addWidget(queryButton);
        buttonLayout->addWidget(clearButton);
        buttonLayout->addStretch();
        
        // Output display
        QGroupBox *outputGroup = new QGroupBox("📋 Records & Results", this);
        QVBoxLayout *outputLayout = new QVBoxLayout(outputGroup);
        
        outputView = new QTextEdit(this);
        outputView->setReadOnly(true);
        outputView->setFontFamily("Monospace");
        outputLayout->addWidget(outputView);
        
        // Assemble main layout
        mainLayout->addWidget(inputGroup);
        mainLayout->addWidget(buttonPanel);
        mainLayout->addWidget(outputGroup, 1); // Stretch factor for output area
        
        // Connect signals and slots
        connect(addButton, &QPushButton::clicked, this, &WeatherRecordBook::onAddRecord);
        connect(queryButton, &QPushButton::clicked, this, &WeatherRecordBook::onQueryRecords);
        connect(clearButton, &QPushButton::clicked, this, &WeatherRecordBook::onClearFields);
        
        // Set initial focus
        temperatureEdit->setFocus();
        
        // Show welcome message
        outputView->setPlainText(
            "🌦️ Welcome to Weather Record Book!\n\n"
            "Instructions:\n"
            "• Fill in weather details and click 'Add Record'\n"
            "• Query historical data by date/time using 'Query Records'\n"
            "• Travel suitability is automatically assessed\n\n"
            "Current capacity: 10 records"
        );
    }

    ~WeatherRecordBook() override = default;
};

#include "main.moc"  // Required for Qt's meta-object compiler

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application attributes for better appearance
    QApplication::setApplicationName("WeatherRecordBook");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("QtWeatherApps");
    
    // Enable high-DPI scaling
    #if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
        QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    #endif
    
    WeatherRecordBook window;
    window.show();
    
    return QApplication::exec();
}
