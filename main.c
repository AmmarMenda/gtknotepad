#include <gtk/gtk.h>
#include <stdio.h>

static void file_opened(GObject *object, GAsyncResult *res, gpointer user_data) {
    GtkFileDialog *dialog = GTK_FILE_DIALOG(object);
    GFile *file = NULL;
    GError *error = NULL;

    file = gtk_file_dialog_open_finish(dialog, res, &error);

    if (error) {
        g_print("Error opening file: %s\n", error->message);
        g_error_free(error);
        return;  // Exit the function on error
    }

    if (file) {
        char *filename = g_file_get_path(file);
        GtkTextView *text_view = GTK_TEXT_VIEW(user_data);
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);

        if (filename) {
            FILE *f = fopen(filename, "r");
            if (f != NULL) {
                fseek(f, 0, SEEK_END);
                long file_size = ftell(f);
                rewind(f);

                char *file_contents = g_malloc(file_size + 1);
                if (file_contents == NULL) {
                    g_print("Memory allocation failed!\n");
                    fclose(f);
                    g_free(filename);
                    g_object_unref(file);
                    return;
                }

                fread(file_contents, 1, file_size, f);
                file_contents[file_size] = '\0';

                gtk_text_buffer_set_text(buffer, file_contents, -1);

                fclose(f);
                g_free(file_contents);
            } else {
                g_print("Error opening file for reading.\n");
            }
            g_free(filename);
        }
        g_object_unref(file); // Unref the GFile
    }
}

static void open_file_dialog(GtkWidget *widget, gpointer user_data) {
    GtkFileDialog *dialog;
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_root(GTK_WIDGET(widget)));

    dialog = GTK_FILE_DIALOG(gtk_file_dialog_new());
    gtk_file_dialog_open(dialog, GTK_WINDOW(window), NULL, file_opened, user_data);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkTextView *text_view;
    GtkWidget *scrolled_window;
    GtkWidget *open_button;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "GPad");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);

    scrolled_window = gtk_scrolled_window_new();
    gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 0, 1, 1);

    gtk_widget_set_hexpand(scrolled_window, TRUE);
    gtk_widget_set_vexpand(scrolled_window, TRUE);

    text_view = GTK_TEXT_VIEW(gtk_text_view_new());
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), GTK_WIDGET(text_view));

    open_button = gtk_button_new_with_label("Open");
    g_signal_connect(open_button, "clicked", G_CALLBACK(open_file_dialog), text_view);
    gtk_grid_attach(GTK_GRID(grid), open_button, 0, 1, 1, 1);

    gtk_widget_show(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
