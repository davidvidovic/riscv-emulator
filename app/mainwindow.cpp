#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QDebug>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <QTextDocument>
#include <QTextBlock>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}



int MainWindow::on_pushButton_pressed()
{
    QString text = ui->code_textbox->toPlainText();

    QFile file(QStringLiteral("../input.c")); // define the file to write
    if (file.open(QIODevice::WriteOnly)) // open the file and check that everything is ok
    {        file.write(text.toUtf8()); // write data in the file
        file.close(); // close the file
    }

    // Define the path to the script
    const char* script_dir = "../compiler";
    const char* script = "./run.sh";
    const char* original_dir = "../app";

    // Change the current working directory to the script's directory
    if (chdir(script_dir) != 0) {
        //std::cerr << "Failed to change directory to " << script_dir << std::endl;
        return 1;
    }

    // Call the bash script
    int ret = system(script);

    // Check if the script executed successfully
    if (ret == -1) {
        // system() failed
        //std::cerr << "System call failed" << std::endl;
        return 1;
    } else {
        // system() succeeded, check return status of script
        if (WIFEXITED(ret) && WEXITSTATUS(ret) == 0) {
            // Script executed and exited successfully
            //std::cout << "Script executed successfully" << std::endl;
            //return 0;
        } else {
            // Script executed but failed
            //std::cerr << "Script execution failed with status " << WEXITSTATUS(ret) << std::endl;
            return 1;
        }
    }


    if (chdir(original_dir) != 0) {
        //std::cerr << "Failed to change back to original directory" << std::endl;
        return 1;
    }

    // Print output.asm into asm_textbox
    QFile asm_file(QStringLiteral("../output.s"));
    asm_file.open(QIODevice::ReadOnly);
    QTextStream stream(&asm_file);
    QString content = stream.readAll();
    asm_file.close();
    ui->asm_textbox->setText(content);

    return 0;
}



/* Highlight selected row in code_textbox */

void MainWindow::on_code_textbox_cursorPositionChanged()
{
    QTextCursor cursor = ui->code_textbox->textCursor();
    QTextEdit::ExtraSelection sel;
    sel.cursor = cursor;
    QTextCharFormat fmt;
    fmt.setBackground(QColor(220,220,255));
    fmt.setProperty(QTextFormat::FullWidthSelection, true); // this is important
    sel.format = fmt;
    QList<QTextEdit::ExtraSelection> sels;
    sels << sel;
    ui->code_textbox->setExtraSelections(sels);

    // std:: cout << cursor.blockNumber() << std::endl;



    /* Highlight all rows in asm_textbox that have the same value as cursor.blockNumber()+1 in the file "../compiler/lookup_rosw.txt" */

    QFile lookup_file(QStringLiteral("../compiler/lookup_rows.txt"));
    lookup_file.open(QIODevice::ReadOnly);
    QTextStream stream(&lookup_file);
    QString lookup_content = stream.readAll();
    lookup_file.close();

    int row_cnt = 1;
    QList<QTextEdit::ExtraSelection> selsi;
    for(auto chr : lookup_content)
    {
        if(chr.isDigit())
        {
            if(chr.digitValue() == (cursor.blockNumber() + 1))
            {
                // Highlight that row
                QTextEdit::ExtraSelection sel;
                QTextBlock block(ui->asm_textbox->document()->findBlockByLineNumber(row_cnt-1));
                QTextCursor cursor(block);
                sel.cursor = cursor;
                QTextCharFormat fmt;
                fmt.setBackground(QColor(220,220,255));
                fmt.setProperty(QTextFormat::FullWidthSelection, true); // this is important
                sel.format = fmt;

                selsi << sel;

            }

            row_cnt++;
        }
    }
    ui->asm_textbox->setExtraSelections(selsi);


}

