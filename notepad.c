#include<gtk/gtk.h>
#include "resources.h"
static void print_hello (GtkWidget *widget,
             gpointer   data)
{
  g_print ("YOLO\n");
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *grid;
  GtkTextView *text_view;
  GtkTextBuffer *buffer;
  GtkWidget *scrolled_window;
  GtkWidget *menubar;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "GPad");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);
  
  grid = gtk_grid_new();
  gtk_window_set_child (GTK_WINDOW (window),grid);
  
  GtkBuilder *builder = gtk_builder_new ();
  gtk_builder_add_from_file (builder, "menu.ui", NULL); // Replace with your resource path

  if (g_file_test("builder.ui", G_FILE_TEST_EXISTS)) { // Or the relative/absolute path
    g_print("builder.ui file found.\n");
} else {
    g_print("builder.ui file NOT found!\n");
}

  menubar = GTK_WIDGET(gtk_builder_get_object(builder, "menubar"));
  gtk_grid_attach(GTK_GRID(grid), menubar, 0, 0, 1, 1);
  
  scrolled_window = gtk_scrolled_window_new();
  gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 0, 1, 1);

  gtk_widget_set_hexpand(scrolled_window, TRUE);
  gtk_widget_set_vexpand(scrolled_window, TRUE);
    
  text_view = GTK_TEXT_VIEW(gtk_text_view_new());
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD); // Enable word wrapping
  
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), GTK_WIDGET(text_view)); // Add text_view to scrolled_window
  
  buffer = gtk_text_view_get_buffer(text_view);
  gtk_window_present (GTK_WINDOW (window));
  g_object_unref (builder);
}

int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
