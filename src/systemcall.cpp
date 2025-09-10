#include "headers/systemcall.h"

/** 
 *  
 *  @return void 
 *  */
void SystemCall::resetBtnInit()
{
    initBtn(0);
}

/** 
 *  
 *  @return void
 *  @var action 0| 1  
 *  */
void SystemCall::initBtn(int action, string message)
{
    QString statusMessage = QString::fromStdString(message);

    switch (action) {
    case 0:
        if (isTracingRunning->isChecked()) {
            iptPidRunning->setEnabled(true);
            selectProgrammBtn->setEnabled(false);
            iptProgramm->setEnabled(false);
        } else {
            iptPidRunning->setEnabled(false);
            selectProgrammBtn->setEnabled(true);
            iptProgramm->setEnabled(true);
        }
        isTracingRunning->setEnabled(true);
        statusLabel->setText(!statusMessage.isEmpty() ? statusMessage : "En attente");
        startButton->setEnabled(true);
        stopButton->setEnabled(false);
        traceTypeComboBox->setEnabled(true);
        statusLabel->setStyleSheet("color: #fff; font-weight: bold; font-size: 20px; padding:10px; "
                                   "background-color:orange;");
        break;
    case 1:
        isTracingRunning->setEnabled(false);
        isTracingRunning->setEnabled(false);
        iptPidRunning->setEnabled(false);
        selectProgrammBtn->setEnabled(false);
        iptProgramm->setEnabled(false);
        statusLabel->setText(!statusMessage.isEmpty() ? statusMessage : "Traçage En Cours");
        startButton->setEnabled(false);
        stopButton->setEnabled(true);
        traceTypeComboBox->setEnabled(false);
        statusLabel->setStyleSheet("color: #fff; font-weight: bold; font-size: 20px; padding:10px; "
                                   "background-color:green;");
        break;
    }
}

SystemCall::SystemCall(QWidget *parent)
    : QMainWindow(parent)
    , child_pid(-1)
{
    auto *mainWidget = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(mainWidget);

    // Status Label at the top
    statusLabel = new QLabel("En Attente", this);
    statusLabel->setStyleSheet(
        "color: #fff; font-weight: bold; font-size: 20px; padding:10px; background-color:orange;");
    statusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(statusLabel);

    auto *mainContentOptions = new QVBoxLayout();

    // Tracing Options Layout
    auto *tracingOptionsLayout = new QHBoxLayout();
    isTracingRunning = new QCheckBox("Tracer un Processus déjà en cours", this);
    iptPidRunning = new QLineEdit(this);
    tracingOptionsLayout->addWidget(isTracingRunning);
    tracingOptionsLayout->addWidget(iptPidRunning);
    mainContentOptions->addLayout(tracingOptionsLayout);

    // Program Selection Layout
    auto *programSelectionLayout = new QHBoxLayout();
    selectProgrammBtn = new QPushButton("Sélectionner le programme", this);
    iptProgramm = new QLineEdit("/bin/xterm", this);
    programSelectionLayout->addWidget(selectProgrammBtn);
    programSelectionLayout->addWidget(iptProgramm);
    mainContentOptions->addLayout(programSelectionLayout);

    traceTypeComboBox = new QComboBox(this);
    traceTypeComboBox->addItem("All", "all");
    traceTypeComboBox->addItem("File", "file");
    traceTypeComboBox->addItem("Process", "process");
    traceTypeComboBox->addItem("Network", "network");
    traceTypeComboBox->addItem("Signal", "signal");
    traceTypeComboBox->addItem("IPCF", "ipc");
    traceTypeComboBox->setFixedWidth(100);
    traceTypeComboBox->setStyleSheet(
        "background-color: #3498db; color: white; padding: 5px; border-radius: 5px;");
    auto *vLayoutFilter = new QVBoxLayout();

    auto label = new QLabel("Filtre", this);
    vLayoutFilter->addWidget(label);
    vLayoutFilter->addWidget(traceTypeComboBox);

    auto *contentOptions = new QHBoxLayout();
    contentOptions->addLayout(mainContentOptions);
    contentOptions->addLayout(vLayoutFilter);
    mainLayout->addLayout(contentOptions);

    // Control Buttons Layout
    auto *controlButtonsLayout = new QHBoxLayout();
    startButton = new QPushButton("Lancer le traçage", this);
    stopButton = new QPushButton("Arrêter le traçage", this);
    clearBtn = new QPushButton("Clear", this);

    startButton->setStyleSheet("QPushButton { background-color: green; color: white; font-weight: "
                               "bold; } QPushButton:disabled { background-color: rgba(0, 128, 0, "
                               "0.2); color: rgba(255, 255, 255, 0.5); }");
    stopButton->setStyleSheet("QPushButton { background-color: red; color: white; font-weight: "
                              "bold; } QPushButton:disabled { background-color: rgba(255, 0, 0, "
                              "0.2); color: rgba(255, 255, 255, 0.5); }");
    clearBtn->setStyleSheet("QPushButton { background-color: blue; color: white; font-weight: "
                            "bold; } QPushButton:disabled { background-color: rgba(0, 0, 255, "
                            "0.2); color: rgba(255, 255, 255, 0.5); }");

    controlButtonsLayout->addWidget(startButton);
    controlButtonsLayout->addWidget(stopButton);
    controlButtonsLayout->addWidget(clearBtn);
    mainLayout->addLayout(controlButtonsLayout);

    // Syscall Table
    syscallTable = new QTableWidget(this);
    syscallTable->setColumnCount(5);
    syscallTable->setHorizontalHeaderLabels(
        {"ID", "Heure", "Appel système", "Arguments", "Résultat"});
    syscallTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    syscallTable->horizontalHeader()->setStyleSheet(
        "background-color: #000; color: #fff; font-weight: bold;");
    syscallTable->setStyleSheet(
        "QTableWidget::item { background-color: #d3d3d3; } QTableWidget::item:selected { "
        "background-color: #fff; color:#000; }");
    syscallTable->setAlternatingRowColors(true);
    syscallTable->setStyleSheet(syscallTable->styleSheet()
                                + "QTableWidget { alternate-background-color: #f0f0f0; }");

    mainLayout->addWidget(syscallTable);

    setCentralWidget(mainWidget);

    process = new QProcess(this);
    connect(selectProgrammBtn, SIGNAL(clicked()), this, SLOT(selectProgram()));
    connect(startButton, SIGNAL(clicked()), this, SLOT(startTracing()));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(stopTracing()));
    connect(clearBtn, SIGNAL(clicked()), this, SLOT(setClearBtn()));

    connect(isTracingRunning, SIGNAL(clicked()), this, SLOT(resetBtnInit()));

    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStraceOutput()));
    // connect(process, SIGNAL(finished()), this, SLOT(stopTracing()));

    mainWidget->setStyleSheet(" background-color:  #f4f4f5 ; color: black; font-weight: bold;  ");
    initBtn(0);
    // setCentralWidget(mainWidget);
}

void SystemCall::selectProgram()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Choisir le programme à tracer");
    if (!filePath.isEmpty()) {
        iptProgramm->setText(filePath);
    }
}

void SystemCall::startTracing()
{
    cout << "En cours de traçage" << endl;
    QString outputFile = "/dev/stdout";
    // QString outputFile = "log_test.txt";
    QStringList arguments;
    QString traceType = traceTypeComboBox->currentData().toString();
    arguments << "-f" << "-t" << "-v" << "-e" << "trace=" + traceType << "-o" << outputFile;

    if (isTracingRunning->isChecked()) {
        QString pidText = iptPidRunning->text();

        if (pidText.isEmpty()) {
            QMessageBox::warning(this, "Erreur", "Veuillez entrer le PID d'un processus en cours.");
            return;
        }

        bool ok;
        int pid = pidText.toInt(&ok);
        if (!ok || pid <= 0) {
            QMessageBox::warning(this, "Erreur", "Le PID entré n'est pas valide.");
            return;
        }

        // if (!(kill(pid, 0) == 0)) {
        //     QMessageBox::warning(this, "Erreur", "Le processus spécifié n'est pas actif.");
        //     return;
        // }

        arguments << "-p" << QString::number(pid);

        process->start("sudo strace", arguments);
        process->write("pandora_97");
        child_pid = pid; // Attacher au processus existant
        initBtn(1, "Traçage du processus existant en cours");
    } else {
        QString targetApp = iptProgramm->text();

        if (targetApp.isEmpty()) {
            QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un programme.");
            return;
        }

        QFileInfo fileInfo(targetApp);
        if (!fileInfo.exists() || !fileInfo.isExecutable()) {
            QMessageBox::warning(this,
                                 "Erreur",
                                 "Le fichier sélectionné n'est pas exécutable ou n'existe pas.");
            return;
        }

        arguments << targetApp;

        process->start("strace", arguments);
        if (!process->waitForStarted()) {
            QMessageBox::critical(this, "Erreur", "Impossible de démarrer le traçage du processus.");
            return;
        }
        child_pid = process->processId();
        initBtn(1, "Traçage du programme lancé en cours");
    }
}

int SystemCall::stopTracing(bool force)
{

    QMessageBox::StandardButton action
        = QMessageBox::question(this,
                                "Arrêt du traçage",
                                "Voulez-vous  Vraiment Arrêt le tracage ?",
                                QMessageBox::Yes | QMessageBox::No);

    if (process->state() == QProcess::Running) {
        if (action == QMessageBox::Yes) {
            process->kill();
            process->waitForFinished();
            QProcess::execute("kill", {"-9", QString::number(child_pid)});
            cout << "Traçage arrêté " << child_pid << endl;
            initBtn(0, "Tracage arreter ");
        }
    } else {
        cout << "Aucun traçage en cours" << endl;
        initBtn(0, "Aucun traçage en cours");
    }
    return 0;
}

void SystemCall::readStraceOutput()
{
    startButton->setEnabled(false);
    statusLabel->setText("Traçage En cours");
    while (process->canReadLine()) {
        QString line = process->readLine();

        if (line.contains("+++ exited with")) {
            std::cout << "Le processus traqué a été fermé." << kill(child_pid, 0) << std::endl;
            // stopTracing(true);
            // return;
        }

        // Expression régulière pour analyser la sortie de strace
        QRegularExpression re(R"(^(\d+)\s+(\d{2}:\d{2}:\d{2})\s+(\w+)\((.*)\)\s+=\s+(.+)$)");
        QRegularExpressionMatch match = re.match(line);

        cout << "appel" << " : " << line.toStdString() << endl;
        if (match.hasMatch()) {
            QString pid = match.captured(1);
            QString time = match.captured(2);
            QString syscall = match.captured(3);
            QString args = match.captured(4);
            QString result = match.captured(5);

            // Ajouter une ligne dans la table
            int row = syscallTable->rowCount();
            syscallTable->insertRow(row);
            syscallTable->setItem(row, 0, new QTableWidgetItem(pid));
            syscallTable->setItem(row, 1, new QTableWidgetItem(time));
            syscallTable->setItem(row, 2, new QTableWidgetItem(syscall));
            syscallTable->setItem(row, 3, new QTableWidgetItem(args));
            // syscallTable->setItem(row, 4, new QTableWidgetItem(memorySegments));
            syscallTable->setItem(row, 4, new QTableWidgetItem(result));
            syscallTable->scrollToBottom();
        }
    }
}

void SystemCall::setClearBtn()
{
    int length = syscallTable->rowCount();
    for (int i = length; i >= 0; i--) {
        syscallTable->removeRow(i);
    }
}
