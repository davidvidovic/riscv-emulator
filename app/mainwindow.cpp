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
#include <sstream>
#include <string>
#include <fstream>
#include <QRegularExpression>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Load existing input soruce code into editor
    QFile file(QStringLiteral("../input.c"));
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);
        QString content = in.readAll();
        file.close();


        ui->code_textbox->setText(content);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}



int MainWindow::on_pushButton_pressed()
{
    ui->x0reg->clear();
    ui->x1reg->clear();
    ui->x2reg->clear();
    ui->x3reg->clear();
    ui->x4reg->clear();
    ui->x5reg->clear();
    ui->x6reg->clear();
    ui->x7reg->clear();
    ui->x8reg->clear();
    ui->x9reg->clear();
    ui->x10reg->clear();
    ui->x11reg->clear();
    ui->x12reg->clear();
    ui->x13reg->clear();
    ui->x14reg->clear();
    ui->x15reg->clear();
    ui->x16reg->clear();
    ui->x17reg->clear();
    ui->x18reg->clear();
    ui->x19reg->clear();
    ui->x20reg->clear();
    ui->x21reg->clear();
    ui->x22reg->clear();
    ui->x23reg->clear();
    ui->x24reg->clear();
    ui->x25reg->clear();
    ui->x26reg->clear();
    ui->x27reg->clear();
    ui->x28reg->clear();
    ui->x29reg->clear();
    ui->x30reg->clear();
    ui->x31reg->clear();

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


    if (chdir(original_dir) != 0) {
        //std::cerr << "Failed to change back to original directory" << std::endl;
        return 1;
    }


    // Print terminal_output
    QFile terminal_file(QStringLiteral("../compiler/terminal_output.txt"));
    terminal_file.open(QIODevice::ReadOnly);
    QTextStream terminal_stream(&terminal_file);
    QString terminal_content = terminal_stream.readAll();
    terminal_file.close();

    if(terminal_content.compare("Compile success\n") == 0)
    {
        ui->terminal_output->setTextBackgroundColor(QColor(124, 232, 123));

        // Print output.asm into asm_textbox
        QFile asm_file(QStringLiteral("../output.s"));
        asm_file.open(QIODevice::ReadOnly);
        QTextStream stream(&asm_file);
        QString content = stream.readAll();
        asm_file.close();
        ui->asm_textbox->setText(content);

        // Print output.bin into bin_textbox
        QFile bin_file(QStringLiteral("../output.bin"));
        bin_file.open(QIODevice::ReadOnly);
        QTextStream stream_bin(&bin_file);
        QString content_bin = stream_bin.readAll();
        bin_file.close();
        ui->bin_textbox->setText(content_bin);
    }
    else
    {
        ui->terminal_output->setTextBackgroundColor(QColor(254, 126, 120));

        ui->asm_textbox->clear();
    }
    ui->terminal_output->setText(terminal_content);




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

    //std::cout << "FILE:\n" << lookup_content.toStdString() << "***\n" << std::endl;

    int row_cnt = 1;
    QList<QTextEdit::ExtraSelection> selsi;

    QRegularExpression re("\\d+"); // Regular expression to match one or more digits
    QRegularExpressionMatchIterator i = re.globalMatch(lookup_content);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString numberStr = match.captured(0);
        int number = numberStr.toInt();
        //qDebug() << "Parsed number:" << number;
        // Analyze the number here

        if(number == (cursor.blockNumber() + 1))
        {
            // Highlight that row
            //std::cout << "Highlighting " << row_cnt << std::endl;
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

    ui->asm_textbox->setExtraSelections(selsi);


    /* Do the same for binary output */

    QFile bin_lookup_file(QStringLiteral("../compiler/bin_lookup_rows.txt"));
    bin_lookup_file.open(QIODevice::ReadOnly);
    QTextStream bin_stream(&bin_lookup_file);
    QString bin_lookup_content = bin_stream.readAll();
    bin_lookup_file.close();

    row_cnt = 1;
    QList<QTextEdit::ExtraSelection> bin_selsi;

    i = re.globalMatch(bin_lookup_content);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString numberStr = match.captured(0);
        int number = numberStr.toInt();
        //qDebug() << "Parsed number:" << number;
        // Analyze the number here

        if(number == (cursor.blockNumber() + 1))
        {
            // Highlight that row
            //std::cout << "Highlighting " << row_cnt << std::endl;
            QTextEdit::ExtraSelection sel;
            QTextBlock block(ui->bin_textbox->document()->findBlockByLineNumber(row_cnt-1));
            QTextCursor cursor(block);
            sel.cursor = cursor;
            QTextCharFormat fmt;
            fmt.setBackground(QColor(220,220,255));
            fmt.setProperty(QTextFormat::FullWidthSelection, true); // this is important
            sel.format = fmt;

            bin_selsi << sel;
        }

        row_cnt++;
    }

    ui->bin_textbox->setExtraSelections(bin_selsi);

}


void MainWindow::on_run_button_clicked()
{
    /* Make emulator */
    const char* script_dir = "../emulator";
    const char* make_script = "make";
    const char* clean_script = "make clean";
    const char* run_script = "./riscv_exe";
    const char* original_dir = "../app";

    // Change the current working directory to the script's directory
    if (chdir(script_dir) != 0) {
        //std::cerr << "Failed to change directory to " << script_dir << std::endl;
        return;
    }

    // Call the bash script
    int ret = system(make_script);
    ret = system(run_script);
    ret = system(clean_script);


    if (chdir(original_dir) != 0) {
        //std::cerr << "Failed to change back to original directory" << std::endl;
        return;
    }


    // Print register values from "emulator/dump_regs_output.txt" into register textboxes

    QFile dump_reg_file(QStringLiteral("../emulator/dump_regs_output.txt"));
    dump_reg_file.open(QIODevice::ReadOnly);
    QTextStream stream(&dump_reg_file);
    QString content = stream.readAll();
    dump_reg_file.close();

    QStringList Lines = content.split('\n');
    QString regs[33];

    for (int i = 0; i < Lines.size(); i++) {
        regs[i] = Lines[i];
    }

    ui->x0reg->setText(regs[0]);
    ui->x1reg->setText(regs[1]);
    ui->x2reg->setText(regs[2]);
    ui->x3reg->setText(regs[3]);
    ui->x4reg->setText(regs[4]);
    ui->x5reg->setText(regs[5]);
    ui->x6reg->setText(regs[6]);
    ui->x7reg->setText(regs[7]);
    ui->x8reg->setText(regs[8]);
    ui->x9reg->setText(regs[9]);
    ui->x10reg->setText(regs[10]);
    ui->x11reg->setText(regs[11]);
    ui->x12reg->setText(regs[12]);
    ui->x13reg->setText(regs[13]);
    ui->x14reg->setText(regs[14]);
    ui->x15reg->setText(regs[15]);
    ui->x16reg->setText(regs[16]);
    ui->x17reg->setText(regs[17]);
    ui->x18reg->setText(regs[18]);
    ui->x19reg->setText(regs[19]);
    ui->x20reg->setText(regs[20]);
    ui->x21reg->setText(regs[21]);
    ui->x22reg->setText(regs[22]);
    ui->x23reg->setText(regs[23]);
    ui->x24reg->setText(regs[24]);
    ui->x25reg->setText(regs[25]);
    ui->x26reg->setText(regs[26]);
    ui->x27reg->setText(regs[27]);
    ui->x28reg->setText(regs[28]);
    ui->x29reg->setText(regs[29]);
    ui->x30reg->setText(regs[30]);
    ui->x31reg->setText(regs[31]);

}

