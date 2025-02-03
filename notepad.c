#include <gtk/gtk.h>
#include <stdio.h>

static void print_hello(GtkWidget *widget, gpointer data) {
    g_print("YOLO\n");
}

static void file_opened(GObject *object, GAsyncResult *res, gpointer user_data) {
    GtkFileDialog *dialog = GTK_FILE_DIALOG(object);
    GFile *file = NULL;
    GError *error = NULL;

    file = gtk_file_dialog_open_finish(dialog, res, &error); // Get file and error

    if (error) {
        g_print("Error opening file: %s\n", error->message);
        g_error_free(error);
        return;
    }

    if (file) {
        char *filename = g_file_get_path(file);
        GtkTextView *text_view = GTK_TEXT_VIEW(user_data);
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
        GtkTextIter start, end;

        if (filename) {
            FILE *f = fopen(filename, "w");
            if (f != NULL) {
                gtk_text_buffer_get_start_iter(buffer, &start);
                gtk_text_buffer_get_end_iter(buffer, &end);
                char *text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
                fprintf(f, "%s", text);
                fclose(f);
                g_free(text);
            } else {
                g_print("Error opening file for writing.\n");
            }
            g_free(filename);
        }
        g_object_unref(file);
    }
}


static gboolean abort_mission(gpointer user_data) {
    GCancellable *cancellable = (GCancellable*)user_data;
    g_cancellable_cancel(cancellable);
    g_object_unref(cancellable); // Unref after use
    return G_SOURCE_REMOVE; // Remove the timeout source
}


static void save_file(GtkWidget *widget, gpointer user_data) {
    GtkFileDialog *dialog;
    GCancellable *cancellable;
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_root(GTK_WIDGET(widget))); // Explicit cast // Get the window
    const char *label = "Save"; // Label for the dialog

    dialog = GTK_FILE_DIALOG(gtk_file_dialog_new());
    cancellable = g_cancellable_new();

    g_timeout_add_seconds_full(G_PRIORITY_DEFAULT, 20, abort_mission, g_object_ref(cancellable), g_object_unref);
    gtk_file_dialog_open(dialog, GTK_WINDOW(window), cancellable, file_opened, user_data); // Pass user_data

    g_object_unref(cancellable); // Unref after use. Important to avoid memory leaks.
    g_object_unref(dialog); // Unref the dialog. Important to avoid memory leaks.
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkTextView *text_view;
    GtkTextBuffer *buffer;
    GtkWidget *scrolled_window;
    GtkWidget *save_button;
    GtkBuilder *builder;


    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "GPad");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);

    grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);

    scrolled_window = gtk_scrolled_window_new();
    gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 0, 1, 1);

    gtk_widget_set_hexpand(scrolled_window, TRUE);
    gtk_widget_set_vexpand(scrolled_window, TRUE);

    text_view = GTK_TEXT_VIEW(gtk_text_view_new());
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), GTK_WIDGET(text_view));
    buffer = gtk_text_view_get_buffer(text_view);
    gtk_window_present(GTK_WINDOW(window));

    save_button = gtk_button_new_with_label("Save");
    g_signal_connect(save_button, "clicked", G_CALLBACK(save_file), text_view); // Pass text_view
    gtk_grid_attach(GTK_GRID(grid), save_button, 0, 1, 1, 1);
    gtk_widget_set_visible(save_button,TRUE);

    g_object_unref(builder);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), app);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
