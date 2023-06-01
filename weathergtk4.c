#include <stdio.h>
// A graphical program in C language using gtk library
// A weather record book
// Record today's temperature, humidity, weather phenomenon, date, and morning or afternoon
// Judge travel suitability and query the weather stored before

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Define the structure of a weather record
typedef struct {
  int temperature; // in Celsius degree
  int humidity; // in percentage
  char *phenomenon; // e.g. sunny, cloudy, rainy, etc.
  char *date; // in YYYY-MM-DD format
  char *time; // either "morning" or "afternoon"
} Weather;

// Define the structure of a weather record book
typedef struct {
  Weather *records; // an array of weather records
  int size; // the number of records in the array
  int capacity; // the maximum number of records that can be stored in the array
} WeatherBook;

// Define some global variables for the GUI elements
GtkWidget *window;
GtkWidget *grid;
GtkWidget *label_temperature;
GtkWidget *label_humidity;
GtkWidget *label_phenomenon;
GtkWidget *label_date;
GtkWidget *label_time;
GtkWidget *entry_temperature;
GtkWidget *entry_humidity;
GtkWidget *entry_phenomenon;
GtkWidget *entry_date;
GtkWidget *entry_time;
GtkWidget *button_add;
GtkWidget *button_query;
GtkWidget *button_clear;
GtkWidget *text_view;
GtkTextBuffer *text_buffer;

// Define a global variable for the weather book
WeatherBook *book;

// Initialize the weather book with a given capacity
void init_weather_book(int capacity) {
  book = (WeatherBook *)malloc(sizeof(WeatherBook));
  book->records = (Weather *)malloc(sizeof(Weather) * capacity);
  book->size = 0;
  book->capacity = capacity;
}

// Free the memory allocated for the weather book
void free_weather_book() {
  for (int i = 0; i < book->size; i++) {
    free(book->records[i].phenomenon);
    free(book->records[i].date);
    free(book->records[i].time);
  }
  free(book->records);
  free(book);
}

// Add a new weather record to the weather book
// Return 1 if successful, 0 if failed
int add_weather_record(Weather record) {
  if (book->size == book->capacity) {
    // The weather book is full, cannot add more records
    return 0;
  }
  // Copy the record to the weather book
  book->records[book->size].temperature = record.temperature;
  book->records[book->size].humidity = record.humidity;
  book->records[book->size].phenomenon = strdup(record.phenomenon);
  book->records[book->size].date = strdup(record.date);
  book->records[book->size].time = strdup(record.time);
  book->size++;
  return 1;
}

// Query the weather records that match the given date and time
// Return an array of pointers to the matching records, and store the number of matches in the count parameter
// The caller is responsible for freeing the returned array
Weather **query_weather_records(char *date, char *time, int *count) {
  Weather **results = NULL;
  *count = 0;
  for (int i = 0; i < book->size; i++) {
    // Compare the date and time of each record with the given date and time
    if (strcmp(book->records[i].date, date) == 0 && strcmp(book->records[i].time, time) == 0) {
      // A match is found, add it to the results array
      results = (Weather **)realloc(results, sizeof(Weather *) * (*count + 1));
      results[*count] = &book->records[i];
      (*count)++;
    }
  }
  return results;
}

// Judge the travel suitability based on the weather record
// Return a string that describes the suitability level
char *judge_travel_suitability(Weather record) {
  // Define some thresholds for temperature and humidity
  #define LOW_TEMPERATURE -10
  #define HIGH_TEMPERATURE 35
  #define LOW_HUMIDITY 30
  #define HIGH_HUMIDITY 80

  // Check the temperature and humidity ranges
  if (record.temperature < LOW_TEMPERATURE || record.temperature > HIGH_TEMPERATURE) {
    // The temperature is too low or too high, not suitable for travel
    return "Not suitable";
  }
  if (record.humidity < LOW_HUMIDITY || record.humidity > HIGH_HUMIDITY) {
    // The humidity is too low or too high, not comfortable for travel
    return "Not comfortable";
  }
  // Check the weather phenomenon
  if (strcmp(record.phenomenon, "sunny") == 0) {
    // The weather is sunny, suitable for travel
    return "Suitable";
  }
  if (strcmp(record.phenomenon, "cloudy") == 0) {
    // The weather is cloudy, acceptable for travel
    return "Acceptable";
  }
  if (strcmp(record.phenomenon, "rainy") == 0) {
    // The weather is rainy, not ideal for travel
    return "Not ideal";
  }
  if (strcmp(record.phenomenon, "snowy") == 0) {
    // The weather is snowy, not advisable for travel
    return "Not advisable";
  }
  if (strcmp(record.phenomenon, "stormy") == 0) {
    // The weather is stormy, dangerous for travel
    return "Dangerous";
  }
  // The weather phenomenon is unknown, cannot judge the travel suitability
  return "Unknown";
}

// Format a weather record as a string
// Return a newly allocated string that contains the formatted record
// The caller is responsible for freeing the returned string
char *format_weather_record(Weather record) {
  char *result = NULL;
  int len = 0;
  // Append the temperature, humidity, phenomenon, date and time to the result string
  len += asprintf(&result, "Temperature: %d°C\n", record.temperature);
  len += asprintf(&result + len, "Humidity: %d%%\n", record.humidity);
  len += asprintf(&result + len, "Phenomenon: %s\n", record.phenomenon);
  len += asprintf(&result + len, "Date: %s\n", record.date);
  len += asprintf(&result + len, "Time: %s\n", record.time);
  // Judge the travel suitability and append it to the result string
  char *suitability = judge_travel_suitability(record);
  len += asprintf(&result + len, "Travel suitability: %s\n", suitability);
  return result;
}

// Get the current date in YYYY-MM-DD format
// Return a newly allocated string that contains the date
// The caller is responsible for freeing the returned string
char *get_current_date() {
  char *date = NULL;
  time_t t = time(NULL);
  struct tm *tm = localtime(&t);
  asprintf(&date, "%04d-%02d-%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
  return date;
}

// Get the current time as either "morning" or "afternoon"
// Return a newly allocated string that contains the time
// The caller is responsible for freeing the returned string
char *get_current_time() {
  char *time_of_day = NULL;
  time_t t = time(NULL);
  struct tm *tm = localtime(&t);
  if (tm->tm_hour < 12) {
    // The current time is before noon, return "morning"
    asprintf(&time_of_day, "morning");
  } else {
    // The current time is after noon, return "afternoon"
    asprintf(&time_of_day, "afternoon");
  }
  return time_of_day;
}

// A callback function that is invoked when the add button is clicked
void on_button_add_clicked(GtkWidget *widget, gpointer data) {
  // Get the input values from the entry widgets
  const char *temperature = gtk_editable_get_text(GTK_EDITABLE(entry_temperature));
  const char *humidity = gtk_editable_get_text(GTK_EDITABLE(entry_humidity));
  const char *phenomenon = gtk_editable_get_text(GTK_EDITABLE(entry_phenomenon));
  const char *date = gtk_editable_get_text(GTK_EDITABLE(entry_date));
  const char *time_of_day = gtk_editable_get_text(GTK_EDITABLE(entry_time));

  // Validate the input values
  if (strlen(temperature) == 0 || strlen(humidity) == 0 || strlen(phenomenon) == 0 || strlen(date) == 0 || strlen(time_of_day) == 0) {
    // Some input values are empty, show an error message and return
    gtk_text_buffer_set_text(text_buffer, "Please fill in all the fields.",
    -1);
    return;
  }
  if (atoi(temperature) < -273 || atoi(temperature) > 100) {
    // The temperature value is invalid, show an error message and return
    gtk_text_buffer_set_text(text_buffer, "Please enter a valid temperature between -273 and 100.",
    -1);
    return;
  }
  if (atoi(humidity) < 0 || atoi(humidity) > 100) {
    // The humidity value is invalid, show an error message and return
    gtk_text_buffer_set_text(text_buffer, "Please enter a valid humidity between 0 and 100.",
    -1);
    return;
  }
  if (strcmp(time_of_day, "morning") != 0 && strcmp(time_of_day, "afternoon") != 0) {
    // The time value is invalid, show an error message and return
    gtk_text_buffer_set_text(text_buffer, "Please enter either morning or afternoon for the time.",
    -1);
    return;
  }

  // Create a new weather record with the input values
  Weather record;
  record.temperature = atoi(temperature);
  record.humidity = atoi(humidity);
  record.phenomenon = strdup(phenomenon);
  record.date = strdup(date);
  record.time = strdup(time_of_day);

  // Add the new weather record to the weather book
  int success = add_weather_record(record);
  if (success) {
    // The record is added successfully, show a success message
    gtk_text_buffer_set_text(text_buffer, "The weather record is added successfully.",
    -1);
  } else {
    // The record is not added successfully, show an error message
    gtk_text_buffer_set_text(text_buffer, "The weather book is full. Cannot add more records.",
    -1);
  }
}

// A callback function that is invoked when the query button is clicked
void on_button_query_clicked(GtkWidget *widget, gpointer data) {
  // Get the input values from the entry widgets
  const char *date = gtk_editable_get_text(GTK_EDITABLE(entry_date));
  const char *time_of_day = gtk_editable_get_text(GTK_EDITABLE(entry_time));

  // Validate the input values
  if (strlen(date) == 0 || strlen(time_of_day) == 0) {
    // Some input values are empty, show an error message and return
    gtk_text_buffer_set_text(text_buffer, "Please fill in the date and time fields.",
    -1);
    return;
  }
  if (strcmp(time_of_day, "morning") != 0 && strcmp(time_of_day, "afternoon") != 0) {
    // The time value is invalid, show an error message and return
    gtk_text_buffer_set_text(text_buffer, "Please enter either morning or afternoon for the time.",
    -1);
    return;
  }

  // Query the weather records that match the input values
  int count = 0;
  Weather **results = query_weather_records(date, time_of_day, &count);
  if (count == 0) {
    // No matching records are found, show a message and return
    gtk_text_buffer_set_text(text_buffer, "No weather records are found for the given date and time.",
    -1);
    free(results);
    return;
  }

  // Format the matching records as a string and show it in the text view
  char *output = NULL;
  int len = 0;
  for (int i = 0; i < count; i++) {
    char *record = format_weather_record(*results[i]);
    len += asprintf(&output + len, "Record %d:\n%s\n", i + 1, record);
    free(record);
  }
  gtk_text_buffer_set_text(text_buffer, output,
  -1);
  free(output);
  free(results);
}

// A callback function that is invoked when the clear button is clicked
void on_button_clear_clicked(GtkWidget *widget, gpointer data) {
  // Clear the input values from the entry widgets
  gtk_editable_set_text(GTK_EDITABLE(entry_temperature), "");
  gtk_editable_set_text(GTK_EDITABLE(entry_humidity), "");
  gtk_editable_set_text(GTK_EDITABLE(entry_phenomenon), "");
  gtk_editable_set_text(GTK_EDITABLE(entry_date), "");
  gtk_editable_set_text(GTK_EDITABLE(entry_time), "");

  // Clear the text view
  gtk_text_buffer_set_text(text_buffer, "",
  -1);
}

// A callback function that is invoked when the window is closed
void on_window_destroy(GtkWidget *widget, gpointer data) {
  // Free the memory allocated for the weather book
  free_weather_book();

  // Quit the main loop
  g_application_quit(G_APPLICATION(data));
}

// The main function of the program
int main(int argc, char *argv[]) {
  // Initialize the GTK library
  gtk_init();

  // Initialize the weather book with a capacity of 10 records
  init_weather_book(10);

  // Create a window widget
  window = gtk_window_new();
  gtk_window_set_title(GTK_WINDOW(window), "Weather Record Book");
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

  // Create a grid widget to arrange other widgets
  grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
  gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
  gtk_window_set_child(GTK_WINDOW(window), grid);

  // Create some label widgets to display the input fields
  label_temperature = gtk_label_new("Temperature (°C):");
  label_humidity = gtk_label_new("Humidity (%):");
  label_phenomenon = gtk_label_new("Phenomenon:");
  label_date = gtk_label_new("Date (YYYY-MM-DD):");
  label_time = gtk_label_new("Time (morning/afternoon):");

  // Create some entry widgets to get the input values
  entry_temperature = gtk_entry_new();
  entry_humidity = gtk_entry_new();
  entry_phenomenon = gtk_entry_new();
  entry_date = gtk_entry_new();
  entry_time = gtk_entry_new();

  // Set the default values for the date and time entries
  char *current_date = get_current_date();
  char *current_time = get_current_time();
  gtk_editable_set_text(GTK_EDITABLE(entry_date), current_date);
  gtk_editable_set_text(GTK_EDITABLE(entry_time), current_time);
  free(current_date);
  free(current_time);

  // Create some button widgets to perform actions
  button_add = gtk_button_new_with_label("Add");
  button_query = gtk_button_new_with_label("Query");
  button_clear = gtk_button_new_with_label("Clear");

  // Connect the button's clicked signals to the callback functions
  g_signal_connect(button_add, "clicked", G_CALLBACK(on_button_add_clicked), NULL);
  g_signal_connect(button_query, "clicked", G_CALLBACK(on_button_query_clicked), NULL);
  g_signal_connect(button_clear, "clicked", G_CALLBACK(on_button_clear_clicked), NULL);

  // Create a text view widget to display the output messages
  text_view = gtk_text_view_new();
  text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
  gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), FALSE);

  // Create a scrolled window widget to contain the text view
  GtkWidget *scrolled_window = gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), text_view);

  // Attach the widgets to the grid
  gtk_grid_attach(GTK_GRID(grid), label_temperature, 0, 0, 1, 1);
}
