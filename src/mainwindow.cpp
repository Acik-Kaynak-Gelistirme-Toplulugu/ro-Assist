#include "roassist/mainwindow.h"
#include "roassist/ui_texts.h"
#include "roassist/update_helpers.h"

#include <algorithm>

#include <QDesktopServices>
#include <QDialog>
#include <QEasingCurve>
#include <QEvent>
#include <QFile>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QGridLayout>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLocale>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPalette>
#include <QPainterPath>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QProcessEnvironment>
#include <QScreen>
#include <QScrollBar>
#include <QScrollArea>
#include <QSettings>
#include <QStandardPaths>
#include <QStyleHints>
#include <QUrl>
#include <QVariantAnimation>
#include <QVBoxLayout>
#include <QtConcurrentRun>

namespace {

QString localeValueFromFile(const QString &path) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return {};

  while (!file.atEnd()) {
    const QString line = QString::fromUtf8(file.readLine()).trimmed();
    if (line.startsWith(QStringLiteral("LANGUAGE=")) ||
        line.startsWith(QStringLiteral("LANG="))) {
      QString value = line.section(QLatin1Char('='), 1).trimmed();
      value.remove(QLatin1Char('"'));
      value.remove(QLatin1Char('\''));
      return value.section(QLatin1Char(':'), 0, 0);
    }
  }
  return {};
}

QString desktopLocaleName() {
  for (const char *variable : {"LANGUAGE", "LC_ALL", "LC_MESSAGES", "LANG"}) {
    const QString value = qEnvironmentVariable(variable).trimmed();
    if (!value.isEmpty() && !value.startsWith(QStringLiteral("C")) &&
        !value.startsWith(QStringLiteral("POSIX"))) {
      return value.section(QLatin1Char(':'), 0, 0);
    }
  }

  const QLocale locale = QLocale::system();
  if (locale.language() != QLocale::C)
    return locale.name();

  const QString configDirectory =
      QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
  for (const QString &path :
       {configDirectory + QStringLiteral("/plasma-localerc"),
        configDirectory + QStringLiteral("/kdeglobals"),
        QStringLiteral("/etc/locale.conf")}) {
    const QString value = localeValueFromFile(path);
    if (!value.isEmpty())
      return value;
  }
  return {};
}

MainWindow::Language languageForLocaleName(const QString &localeName) {
  const QString language = localeName.section(QLatin1Char('_'), 0, 0)
                               .section(QLatin1Char('-'), 0, 0)
                               .section(QLatin1Char('.'), 0, 0)
                               .toLower();
  if (language == QStringLiteral("tr"))
    return MainWindow::TR;
  if (language == QStringLiteral("es"))
    return MainWindow::ES;
  if (language == QStringLiteral("de"))
    return MainWindow::DE;
  if (language == QStringLiteral("fr"))
    return MainWindow::FR;

  // English is the complete fallback catalogue. Never present a partially
  // translated interface when the desktop language is not shipped yet.
  return MainWindow::EN;
}

int boundedTelemetryLevel(int level) {
  if (level < 0)
    return 0;
  if (level > 3)
    return 3;
  return level;
}

QString telemetryLevelSettingName(int level) {
  switch (boundedTelemetryLevel(level)) {
  case 0:
    return QStringLiteral("off");
  case 2:
    return QStringLiteral("basic");
  case 3:
    return QStringLiteral("extended");
  case 1:
  default:
    return QStringLiteral("count");
  }
}

QColor mixColor(const QColor &a, const QColor &b, qreal amount) {
  const qreal t = std::clamp(amount, 0.0, 1.0);
  return QColor::fromRgbF(a.redF() + (b.redF() - a.redF()) * t,
                          a.greenF() + (b.greenF() - a.greenF()) * t,
                          a.blueF() + (b.blueF() - a.blueF()) * t,
                          a.alphaF() + (b.alphaF() - a.alphaF()) * t);
}

QString cssColor(const QColor &color) {
  return QStringLiteral("rgba(%1,%2,%3,%4)")
      .arg(color.red())
      .arg(color.green())
      .arg(color.blue())
      .arg(color.alpha());
}

QScrollArea *makeScrollablePanel(QWidget *parent, QWidget *content,
                                 const QString &objectName) {
  auto *scrollArea = new QScrollArea(parent);
  scrollArea->setObjectName(objectName);
  scrollArea->setWidgetResizable(true);
  scrollArea->setFrameShape(QFrame::NoFrame);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  scrollArea->setWidget(content);
  return scrollArea;
}

struct ThemeTokens {
  QColor text;
  QColor muted;
  QColor pageStart;
  QColor pageMid;
  QColor pageEnd;
  QColor glassStart;
  QColor glassEnd;
  QColor glassStrongStart;
  QColor glassStrongEnd;
  QColor border;
  QColor borderHot;
  QColor accent;
  QColor accentTwo;
  QColor accentThree;
  QColor shadow;
};

ThemeTokens tokensForTheme(bool dark) {
  if (dark) {
    return {QColor("#f5f7fa"), QColor("#b9bec8"), QColor("#0b0d10"),
            QColor("#171a20"), QColor("#23272f"), QColor(255, 255, 255, 36),
            QColor(255, 255, 255, 18), QColor(255, 255, 255, 54),
            QColor(255, 255, 255, 26), QColor(255, 255, 255, 42),
            QColor(255, 255, 255, 120), QColor("#303640"),
            QColor("#5d6672"), QColor("#8b929d"), QColor(0, 0, 0, 190)};
  }
  return {QColor("#111317"), QColor("#5f6570"), QColor("#f6f7f8"),
          QColor("#ebecef"), QColor("#d7dae0"), QColor(255, 255, 255, 216),
          QColor(255, 255, 255, 148), QColor(255, 255, 255, 236),
          QColor(245, 246, 248, 176), QColor(255, 255, 255, 220),
          QColor(20, 22, 26, 108), QColor("#111317"), QColor("#424850"),
          QColor("#767c86"), QColor(16, 18, 22, 64)};
}

class GradientBackground final : public QWidget {
public:
  explicit GradientBackground(bool dark, QWidget *parent = nullptr)
      : QWidget(parent), darkTheme(dark) {
    setAutoFillBackground(false);
    setAttribute(Qt::WA_StyledBackground, false);
  }

  void setDarkTheme(bool dark) {
    darkTheme = dark;
    update();
  }

protected:
  void paintEvent(QPaintEvent *) override {
    const ThemeTokens t = tokensForTheme(darkTheme);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QLinearGradient base(rect().topLeft(), rect().bottomRight());
    base.setColorAt(0.0, t.pageStart);
    base.setColorAt(0.48, t.pageMid);
    base.setColorAt(1.0, t.pageEnd);
    painter.fillRect(rect(), base);

    const int w = width();
    const int h = height();
    auto drawBand = [&](qreal yOffset, const QColor &color, qreal alpha,
                        qreal thickness) {
      QPainterPath band;
      band.moveTo(-w * 0.15, h * yOffset);
      band.lineTo(w * 1.12, h * (yOffset - 0.18));
      band.lineTo(w * 1.18, h * (yOffset - 0.18) + thickness);
      band.lineTo(-w * 0.1, h * yOffset + thickness);
      band.closeSubpath();
      QColor c = color;
      c.setAlphaF(alpha);
      painter.fillPath(band, c);
    };
    drawBand(0.22, darkTheme ? QColor("#ffffff") : QColor("#000000"),
             darkTheme ? 0.045 : 0.035, h * 0.11);
    drawBand(0.72, darkTheme ? QColor("#ffffff") : QColor("#000000"),
             darkTheme ? 0.035 : 0.028, h * 0.15);

    QLinearGradient veil(rect().topLeft(), rect().bottomLeft());
    veil.setColorAt(0.0, QColor(255, 255, 255, darkTheme ? 8 : 42));
    veil.setColorAt(1.0, QColor(255, 255, 255, 0));
    painter.fillRect(rect(), veil);
  }

private:
  bool darkTheme;
};

class GlassPanel final : public QWidget {
public:
  enum Strength { Soft, Strong };

  explicit GlassPanel(bool dark, Strength strength = Soft,
                      QWidget *parent = nullptr)
      : QWidget(parent), darkTheme(dark), panelStrength(strength),
        shadowEffect(new QGraphicsDropShadowEffect(this)) {
    setAutoFillBackground(false);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_StyledBackground, false);
    setGraphicsEffect(shadowEffect);
    updateShadow();
  }

  void setDarkTheme(bool dark) {
    darkTheme = dark;
    updateShadow();
    update();
  }

protected:
  void paintEvent(QPaintEvent *) override {
    const ThemeTokens t = tokensForTheme(darkTheme);
    const QRectF r = rect().adjusted(2, 2, -2, -3);
    const qreal radius = 26;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QLinearGradient fill(r.topLeft(), r.bottomRight());
    fill.setColorAt(0.0, panelStrength == Strong ? t.glassStrongStart
                                                 : t.glassStart);
    fill.setColorAt(0.52, mixColor(t.glassStart, t.glassEnd, 0.35));
    fill.setColorAt(1.0, panelStrength == Strong ? t.glassStrongEnd
                                                 : t.glassEnd);
    QPainterPath path;
    path.addRoundedRect(r, radius, radius);
    painter.fillPath(path, fill);

    QLinearGradient shine(r.topLeft(), r.bottomLeft());
    shine.setColorAt(0.0, QColor(255, 255, 255, darkTheme ? 48 : 120));
    shine.setColorAt(0.32, QColor(255, 255, 255, 0));
    painter.fillPath(path, shine);

    QPen border(t.border, 1.2);
    painter.setPen(border);
    painter.drawPath(path);
  }

private:
  void updateShadow() {
    const ThemeTokens t = tokensForTheme(darkTheme);
    shadowEffect->setBlurRadius(panelStrength == Strong ? 34 : 26);
    shadowEffect->setOffset(0, panelStrength == Strong ? 18 : 12);
    shadowEffect->setColor(t.shadow);
  }

  bool darkTheme;
  Strength panelStrength;
  QGraphicsDropShadowEffect *shadowEffect;
};

void drawCardGlyph(QPainter &painter, const QRectF &box, const QString &kind,
                   const ThemeTokens &t, bool dark, qreal progress) {
  painter.save();
  const QPointF center = box.center();
  painter.translate(center);
  painter.rotate((kind.size() % 2 == 0 ? 1 : -1) * progress * 7.0);
  painter.scale(1.0 + progress * 0.07, 1.0 + progress * 0.07);
  painter.translate(-center);

  QRectF r = box.adjusted(5, 5, -5, -5);
  QColor glass = dark ? QColor(255, 255, 255, 18) : QColor(0, 0, 0, 14);
  QColor stroke = mixColor(t.muted, t.text, 0.55 + progress * 0.45);
  QColor hot = mixColor(t.borderHot, t.text, progress * 0.35);

  QPainterPath plate;
  plate.addRoundedRect(r, 14, 14);
  painter.fillPath(plate, glass);
  painter.setPen(QPen(mixColor(t.border, t.borderHot, progress), 1.1));
  painter.drawPath(plate);

  const QRectF g = r.adjusted(13, 12, -13, -12);
  QPen pen(stroke, 2.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  painter.setPen(pen);
  painter.setBrush(Qt::NoBrush);

  if (kind == QStringLiteral("update")) {
    painter.drawArc(g.adjusted(4, 4, -4, -4), 35 * 16, 285 * 16);
    QPolygonF arrow;
    arrow << QPointF(g.right() - 5, g.top() + 10)
          << QPointF(g.right() + 2, g.top() + 2)
          << QPointF(g.right() + 5, g.top() + 13);
    painter.drawPolyline(arrow);
  } else if (kind == QStringLiteral("social")) {
    const QPointF a(g.left() + 8, g.center().y() - 8);
    const QPointF b(g.center().x() + progress * 5, g.center().y() + 2);
    const QPointF c(g.right() - 7, g.center().y() - 10);
    painter.drawLine(a, b);
    painter.drawLine(b, c);
    painter.setBrush(hot);
    painter.drawEllipse(a, 4.5, 4.5);
    painter.drawEllipse(b, 5.5, 5.5);
    painter.drawEllipse(c, 4.5, 4.5);
  } else if (kind == QStringLiteral("store")) {
    QRectF bag(g.left() + 5, g.top() + 13, g.width() - 10, g.height() - 15);
    painter.drawRoundedRect(bag, 5, 5);
    painter.drawArc(QRectF(g.left() + 16, g.top() + 2, g.width() - 32, 24),
                    0, 180 * 16);
  } else if (kind == QStringLiteral("community")) {
    painter.drawEllipse(QPointF(g.center().x(), g.top() + 13), 6, 6);
    painter.drawEllipse(QPointF(g.left() + 13, g.top() + 22), 5, 5);
    painter.drawEllipse(QPointF(g.right() - 13, g.top() + 22), 5, 5);
    painter.drawArc(QRectF(g.left() + 8, g.top() + 28, g.width() - 16, 22),
                    20 * 16, 140 * 16);
    painter.drawArc(QRectF(g.left(), g.top() + 31, 24, 18), 20 * 16,
                    130 * 16);
    painter.drawArc(QRectF(g.right() - 24, g.top() + 31, 24, 18), 30 * 16,
                    130 * 16);
  } else if (kind == QStringLiteral("printer")) {
    painter.drawRoundedRect(QRectF(g.left() + 5, g.top() + 17,
                                   g.width() - 10, 22),
                            5, 5);
    painter.drawRect(QRectF(g.left() + 14, g.top() + 5, g.width() - 28, 18));
    painter.drawLine(QPointF(g.left() + 16, g.bottom() - 8),
                     QPointF(g.right() - 16, g.bottom() - 8));
  } else if (kind == QStringLiteral("library")) {
    painter.drawRoundedRect(QRectF(g.left() + 7, g.top() + 7, 18, 38), 4, 4);
    painter.drawRoundedRect(QRectF(g.left() + 28, g.top() + 12, 18, 33), 4, 4);
    painter.drawLine(QPointF(g.left() + 12, g.top() + 15),
                     QPointF(g.left() + 21, g.top() + 15));
    painter.drawLine(QPointF(g.left() + 33, g.top() + 20),
                     QPointF(g.left() + 42, g.top() + 20));
  } else if (kind == QStringLiteral("telemetry")) {
    painter.drawLine(QPointF(g.left() + 4, g.bottom() - 4),
                     QPointF(g.right() - 4, g.bottom() - 4));
    const qreal bottom = g.bottom() - 8;
    for (int i = 0; i < 4; ++i) {
      const qreal x = g.left() + 10 + i * 12;
      const qreal height = 10 + i * 6 + progress * (i + 1);
      painter.drawRoundedRect(QRectF(x, bottom - height, 6, height), 3, 3);
    }
  } else {
    painter.drawEllipse(g.adjusted(8, 8, -8, -8));
    painter.drawLine(g.topLeft() + QPointF(12, 12),
                     g.bottomRight() - QPointF(12, 12));
  }

  painter.restore();
}

class AnimatedButton final : public QPushButton {
public:
  enum Role { Card, Primary, Secondary, TopBar, Icon };

  explicit AnimatedButton(Role role, bool dark, QWidget *parent = nullptr)
      : QPushButton(parent), buttonRole(role), darkTheme(dark),
        hoverAnimation(new QVariantAnimation(this)),
        shadowEffect(new QGraphicsDropShadowEffect(this)) {
    setAutoFillBackground(false);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_Hover);
    setCursor(Qt::PointingHandCursor);
    setFlat(true);
    setGraphicsEffect(shadowEffect);

    hoverAnimation->setDuration(170);
    hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(hoverAnimation, &QVariantAnimation::valueChanged, this,
            [this](const QVariant &value) {
              hoverProgress = value.toReal();
              updateShadow();
              update();
            });
    updateShadow();
  }

  void setDarkTheme(bool dark) {
    darkTheme = dark;
    updateShadow();
    update();
  }

protected:
  bool event(QEvent *event) override {
    if (event->type() == QEvent::Enter) {
      animateTo(1.0);
    } else if (event->type() == QEvent::Leave) {
      pressed = false;
      animateTo(0.0);
    } else if (event->type() == QEvent::MouseButtonPress) {
      pressed = true;
      update();
    } else if (event->type() == QEvent::MouseButtonRelease) {
      pressed = false;
      update();
    }
    return QPushButton::event(event);
  }

  void paintEvent(QPaintEvent *) override {
    const ThemeTokens t = tokensForTheme(darkTheme);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRectF r = rect().adjusted(3, 3, -3, -4);
    if (pressed)
      r.translate(0, 1.5);

    const qreal radius = buttonRole == Icon ? r.height() / 2.0
                         : buttonRole == Card ? 18
                         : buttonRole == Primary ? 17
                                               : 12;
    QPainterPath path;
    path.addRoundedRect(r, radius, radius);

    QLinearGradient fill(r.topLeft(), r.bottomRight());
    if (!isEnabled()) {
      fill.setColorAt(0, QColor(255, 255, 255, darkTheme ? 26 : 88));
      fill.setColorAt(1, QColor(255, 255, 255, darkTheme ? 16 : 60));
    } else if (buttonRole == Primary) {
      const QColor primaryA = darkTheme ? QColor("#343942") : QColor("#111317");
      const QColor primaryB = darkTheme ? QColor("#656c76") : QColor("#3a3f47");
      const QColor primaryHot =
          darkTheme ? QColor("#858c96") : QColor("#000000");
      fill.setColorAt(0.0, mixColor(primaryA, primaryB, hoverProgress * 0.25));
      fill.setColorAt(0.52,
                      mixColor(primaryB, primaryHot, hoverProgress * 0.3));
      fill.setColorAt(1.0, primaryB);
    } else {
      fill.setColorAt(0.0, mixColor(t.glassStrongStart, t.accentThree,
                                    hoverProgress * 0.2));
      fill.setColorAt(0.55, mixColor(t.glassStart, t.accentTwo,
                                     hoverProgress * 0.18));
      fill.setColorAt(1.0, mixColor(t.glassEnd, t.accent,
                                    hoverProgress * 0.12));
    }
    painter.fillPath(path, fill);

    QColor topShine = QColor(255, 255, 255, darkTheme ? 52 : 132);
    topShine.setAlphaF(topShine.alphaF() + hoverProgress * 0.18);
    QLinearGradient shine(r.topLeft(), r.bottomLeft());
    shine.setColorAt(0.0, topShine);
    shine.setColorAt(0.34, QColor(255, 255, 255, 0));
    painter.fillPath(path, shine);

    painter.setPen(QPen(mixColor(t.border, t.borderHot, hoverProgress), 1.25));
    painter.drawPath(path);

    QFont f = font();
    if (buttonRole == Card)
      f.setWeight(QFont::Bold);
    painter.setFont(f);
    painter.setPen(!isEnabled()
                       ? t.muted
                       : (buttonRole == Primary ? QColor("#ffffff") : t.text));
    QRectF textRect = r.adjusted(buttonRole == Card ? 24 : 14, 8,
                                 buttonRole == Card ? -96 : -14, -8);
    const int flags = buttonRole == Card
                          ? (Qt::AlignVCenter | Qt::AlignLeft |
                             Qt::TextWordWrap)
                          : (Qt::AlignCenter | Qt::TextWordWrap);
    painter.drawText(textRect, flags, text());
    if (buttonRole == Card) {
      const QRectF glyphBox(r.right() - 82, r.center().y() - 33, 62, 62);
      drawCardGlyph(painter, glyphBox, property("cardIcon").toString(), t,
                    darkTheme, hoverProgress);
    }
  }

private:
  void animateTo(qreal target) {
    hoverAnimation->stop();
    hoverAnimation->setStartValue(hoverProgress);
    hoverAnimation->setEndValue(target);
    hoverAnimation->start();
  }

  void updateShadow() {
    const ThemeTokens t = tokensForTheme(darkTheme);
    const qreal boost = hoverProgress;
    shadowEffect->setBlurRadius(buttonRole == Card ? 18 + boost * 18
                                                   : 12 + boost * 12);
    shadowEffect->setOffset(0, buttonRole == Card ? 6 + boost * 6
                                                  : 3 + boost * 4);
    QColor shadow = mixColor(t.shadow, t.accent, boost * 0.38);
    shadow.setAlpha(darkTheme ? 150 : 78);
    shadowEffect->setColor(shadow);
  }

  Role buttonRole;
  bool darkTheme;
  bool pressed = false;
  qreal hoverProgress = 0.0;
  QVariantAnimation *hoverAnimation;
  QGraphicsDropShadowEffect *shadowEffect;
};

void revealStackWidget(QStackedWidget *stack, QWidget *target, bool forward) {
  if (!stack || !target || stack->currentWidget() == target) {
    if (target)
      stack->setCurrentWidget(target);
    return;
  }

  stack->setCurrentWidget(target);
  const QPoint endPos = target->pos();
  target->move(endPos + QPoint(forward ? 34 : -34, 0));
  const bool canFade = target->graphicsEffect() == nullptr;
  QGraphicsOpacityEffect *opacity = nullptr;
  if (canFade) {
    opacity = new QGraphicsOpacityEffect(target);
    opacity->setOpacity(0.0);
    target->setGraphicsEffect(opacity);
  }

  auto *moveAnimation = new QPropertyAnimation(target, "pos", stack);
  moveAnimation->setDuration(220);
  moveAnimation->setEasingCurve(QEasingCurve::OutCubic);
  moveAnimation->setStartValue(target->pos());
  moveAnimation->setEndValue(endPos);

  if (!canFade) {
    QObject::connect(moveAnimation, &QPropertyAnimation::finished,
                     moveAnimation, &QObject::deleteLater);
    moveAnimation->start();
    return;
  }

  auto *fadeAnimation = new QVariantAnimation(stack);
  fadeAnimation->setDuration(220);
  fadeAnimation->setEasingCurve(QEasingCurve::OutCubic);
  fadeAnimation->setStartValue(0.0);
  fadeAnimation->setEndValue(1.0);
  QObject::connect(fadeAnimation, &QVariantAnimation::valueChanged, opacity,
                   [opacity](const QVariant &value) {
                     opacity->setOpacity(value.toReal());
                   });
  QObject::connect(fadeAnimation, &QVariantAnimation::finished, target,
                   [target, opacity]() {
                     Q_UNUSED(opacity);
                     target->setGraphicsEffect(nullptr);
                   });
  QObject::connect(fadeAnimation, &QVariantAnimation::finished, moveAnimation,
                   &QObject::deleteLater);
  QObject::connect(fadeAnimation, &QVariantAnimation::finished, fadeAnimation,
                   &QObject::deleteLater);
  moveAnimation->start();
  fadeAnimation->start();
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), updateProcess(new QProcess(this)),
      checkUpdateProcess(new QProcess(this)),
      checkLibProcess(new QProcess(this)), printerSupportProcess(new QProcess(this)),
      riskWatcher(new QFutureWatcher<RoAssist::SystemRiskSnapshot>(this)),
      welcomeStack(new QStackedWidget(this)),
      logConsole(new QTextEdit(this)),
      libraryLogConsole(new QTextEdit(this)), printerLogConsole(new QTextEdit(this)),
      activeOperation(None),
      currentSystemUpdateStep(0),
      transactionPhaseStarted(false), isTerminatingIntentionally(false),
      systemUpdateHadFailures(false),
      isNetworkConnected(true), isLibraryInstalled(false),
      isPrinterSupportInstalled(false), welcomeCompleted(false) {
  detectSystemLanguageAndTheme();

  QNetworkInformation::loadBackendByFeatures(
      QNetworkInformation::Feature::Reachability);
  if (QNetworkInformation::instance()) {
    isNetworkConnected = QNetworkInformation::instance()->reachability() ==
                         QNetworkInformation::Reachability::Online;
    connect(QNetworkInformation::instance(),
            &QNetworkInformation::reachabilityChanged, this,
            [this](QNetworkInformation::Reachability r) {
              onNetworkConnectedChanged(
                  r == QNetworkInformation::Reachability::Online);
            });
  }

  setupUi();
  connect(riskWatcher, &QFutureWatcher<RoAssist::SystemRiskSnapshot>::finished,
          this, [this]() {
            lastRiskSnapshot = riskWatcher->result();
            riskSnapshotReady = true;
            applyMaintenanceStatus();
          });
  updateUiTextAndImages();
  setupStyle();

  QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                     QStringLiteral("Project-Ro-ASD"),
                     QStringLiteral("ro-assist"));
  welcomeCompleted = settings
                         .value(QStringLiteral("welcome/completed"),
                                settings.value(QStringLiteral("autostart/welcomeShown"),
                                               false))
                         .toBool();
  if (welcomeCompleted)
    showDashboardScreen();
  else
    showWelcomeScreen();

  logConsole->hide();
  libraryLogConsole->hide();
  printerLogConsole->hide();
  if (QScreen *screen = QGuiApplication::primaryScreen()) {
    const QRect availableGeometry = screen->availableGeometry();
    const int width = std::clamp(availableGeometry.width() * 88 / 100, 960,
                                 1440);
    const int height = std::clamp(availableGeometry.height() * 86 / 100, 640,
                                  900);
    resize(std::min(width, availableGeometry.width()),
           std::min(height, availableGeometry.height()));
  }

  connect(updateProcess, &QProcess::readyReadStandardOutput, this,
          &MainWindow::handleUpdateOutput);
  connect(updateProcess, &QProcess::readyReadStandardError, this,
          &MainWindow::handleUpdateErrorOutput);
  connect(updateProcess, &QProcess::finished, this,
          &MainWindow::handleUpdateFinished);
  connect(updateProcess, &QProcess::errorOccurred, this,
          &MainWindow::handleUpdateProcessError);

  connect(checkUpdateProcess, &QProcess::finished, this,
          &MainWindow::handleCheckUpdateFinished);
  connect(checkLibProcess, &QProcess::finished, this,
          &MainWindow::handleCheckLibFinished);
  connect(printerSupportProcess, &QProcess::readyReadStandardOutput, this,
          &MainWindow::handlePrinterSupportOutput);
  connect(printerSupportProcess, &QProcess::readyReadStandardError, this,
          &MainWindow::handlePrinterSupportErrorOutput);
  connect(printerSupportProcess, &QProcess::finished, this,
          &MainWindow::handlePrinterSupportFinished);
  connect(printerSupportProcess, &QProcess::errorOccurred, this,
          &MainWindow::handlePrinterSupportProcessError);

  setInitialUpdateStatus();
  if (qEnvironmentVariableIsEmpty("RO_ASSIST_SKIP_SYSTEM_CHECKS")) {
    statusLabel->setText(RoAssist::UiTexts::buildBundle(
                             currentLanguageCode(), currentTheme == Dark,
                             logConsole->isVisible(),
                             libraryLogConsole->isVisible(), isLibraryInstalled)
                             .checkingUpdates);
    checkUpdateProcess->start("dnf", QStringList() << "check-update");
  }
}

MainWindow::~MainWindow() {
  if (updateProcess && updateProcess->state() != QProcess::NotRunning) {
    updateProcess->terminate();
    if (!updateProcess->waitForFinished(1000)) {
      updateProcess->kill();
      updateProcess->waitForFinished(1000);
    }
  }
  if (checkUpdateProcess &&
      checkUpdateProcess->state() != QProcess::NotRunning) {
    checkUpdateProcess->kill();
    checkUpdateProcess->waitForFinished(1000);
  }
  if (checkLibProcess && checkLibProcess->state() != QProcess::NotRunning) {
    checkLibProcess->kill();
    checkLibProcess->waitForFinished(1000);
  }
  if (printerSupportProcess &&
      printerSupportProcess->state() != QProcess::NotRunning) {
    printerSupportProcess->terminate();
    if (!printerSupportProcess->waitForFinished(1000)) {
      printerSupportProcess->kill();
      printerSupportProcess->waitForFinished(1000);
    }
  }
}

void MainWindow::setOperationRunning(OperationType operation) {
  activeOperation = operation;
  transactionPhaseStarted = false;
  isTerminatingIntentionally = false;
}

void MainWindow::clearActiveOperation() {
  activeOperation = None;
  transactionPhaseStarted = false;
  isTerminatingIntentionally = false;
}

bool MainWindow::isOperationRunning() const {
  return activeOperation != None ||
         updateProcess->state() != QProcess::NotRunning ||
         checkUpdateProcess->state() != QProcess::NotRunning ||
         checkLibProcess->state() != QProcess::NotRunning ||
         printerSupportProcess->state() != QProcess::NotRunning;
}

bool MainWindow::isLibraryOperationActive() const {
  return activeOperation == LibraryInstall;
}

bool MainWindow::isPrinterSupportOperationActive() const {
  return activeOperation == PrinterSupportInstall;
}

QString MainWindow::currentLanguageCode() const {
  switch (currentLang) {
  case TR:
    return QStringLiteral("tr");
  case ES:
    return QStringLiteral("es");
  case DE:
    return QStringLiteral("de");
  case FR:
    return QStringLiteral("fr");
  case EN:
  default:
    return QStringLiteral("en");
  }
}

void MainWindow::detectSystemLanguageAndTheme() {
  currentLang = languageForLocaleName(desktopLocaleName());

  currentTheme = Light;

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
  if (const QStyleHints *hints = QGuiApplication::styleHints()) {
    if (hints->colorScheme() == Qt::ColorScheme::Dark)
      currentTheme = Dark;
  }
#else
  // Fallback for older Qt versions (Qt < 6.5)
  if (QGuiApplication::palette().color(QPalette::WindowText).lightness() >
      QGuiApplication::palette().color(QPalette::Window).lightness()) {
    currentTheme = Dark;
  }
#endif
}

void MainWindow::applySystemLanguage() {
  const Language detectedLanguage = languageForLocaleName(desktopLocaleName());
  if (detectedLanguage == currentLang)
    return;

  currentLang = detectedLanguage;
  updateUiTextAndImages();
}

void MainWindow::setupUi() {
  QWidget *centralWidget = new GradientBackground(currentTheme == Dark, this);
  setCentralWidget(centralWidget);
  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  // TOP BAR
  QWidget *topBarWidget = new QWidget(this);
  QHBoxLayout *topLayout = new QHBoxLayout(topBarWidget);
  topLayout->setContentsMargins(20, 20, 20, 10);
  networkStatusLabel = new QLabel(this);
  networkStatusLabel->setObjectName("networkStatusLabel");
  networkStatusLabel->setStyleSheet(
      "font-weight: bold; color: #ffcc00; font-size: 14px;");
  networkStatusLabel->setVisible(!isNetworkConnected);

  topLayout->addWidget(networkStatusLabel);
  topLayout->addStretch();
  themeToggleBtn =
      new AnimatedButton(AnimatedButton::TopBar, currentTheme == Dark, this);
  themeToggleBtn->setObjectName("themeButton");

  langBtn =
      new AnimatedButton(AnimatedButton::TopBar, currentTheme == Dark, this);
  langBtn->setObjectName("languageButton");
  langBtn->setCursor(Qt::PointingHandCursor);
  langBtn->setFixedSize(130, 42);

  langMenu = new QMenu(langBtn);
  QAction *actTR = langMenu->addAction("🇹🇷 Türkçe");
  actTR->setData(QVariant::fromValue((int)TR));
  QAction *actEN = langMenu->addAction("🇬🇧 English");
  actEN->setData(QVariant::fromValue((int)EN));
  QAction *actES = langMenu->addAction("🇪🇸 Español");
  actES->setData(QVariant::fromValue((int)ES));
  QAction *actDE = langMenu->addAction("🇩🇪 Deutsch");
  actDE->setData(QVariant::fromValue((int)DE));
  QAction *actFR = langMenu->addAction("🇫🇷 Français");
  actFR->setData(QVariant::fromValue((int)FR));

  langBtn->setMenu(langMenu);

  themeToggleBtn->setFixedSize(110, 42);
  connect(themeToggleBtn, &QPushButton::clicked, this,
          &MainWindow::toggleTheme);
  connect(langMenu, &QMenu::triggered, this, &MainWindow::changeLanguageAction);

  topLayout->addWidget(themeToggleBtn);
  topLayout->addWidget(langBtn);
  mainLayout->addWidget(topBarWidget);

  mainStack = new QStackedWidget(this);
  mainStack->setObjectName("mainStack");

  // 1. FIRST-RUN WELCOME FLOW
  welcomeViewWidget = new QWidget(this);
  QVBoxLayout *welcomeLayout = new QVBoxLayout(welcomeViewWidget);
  welcomeLayout->setContentsMargins(36, 12, 36, 26);
  welcomeLayout->setSpacing(12);
  welcomeStack->setObjectName("welcomeStack");
  createWelcomeSlides();

  welcomeProgressLabel = new QLabel(this);
  welcomeProgressLabel->setObjectName("welcomeProgressLabel");
  welcomeProgressLabel->setAlignment(Qt::AlignCenter);

  welcomeNextBtn =
      new AnimatedButton(AnimatedButton::Primary, currentTheme == Dark, this);
  welcomeNextBtn->setObjectName("welcomeNextButton");
  welcomeNextBtn->setMinimumSize(280, 62);
  welcomeNextBtn->setCursor(Qt::PointingHandCursor);
  connect(welcomeNextBtn, &QPushButton::clicked, this,
          &MainWindow::advanceWelcome);

  welcomeLayout->addWidget(welcomeStack, 1);
  welcomeLayout->addWidget(welcomeProgressLabel, 0, Qt::AlignCenter);
  welcomeLayout->addWidget(welcomeNextBtn, 0, Qt::AlignCenter);

  // 2. UPDATE VIEW
  updateViewWidget = new QWidget(this);
  QVBoxLayout *updateLayout = new QVBoxLayout(updateViewWidget);
  updateLayout->setContentsMargins(20, 0, 20, 0);
  backToCarouselBtn =
      new AnimatedButton(AnimatedButton::Secondary, currentTheme == Dark, this);
  backToCarouselBtn->setObjectName("backToHomeButton");
  backToCarouselBtn->setMinimumSize(120, 40);
  backToCarouselBtn->setCursor(Qt::PointingHandCursor);
  connect(backToCarouselBtn, &QPushButton::clicked, this,
          &MainWindow::showHomeScreen);

  QHBoxLayout *updateTopLayout = new QHBoxLayout();
  updateTopLayout->addWidget(backToCarouselBtn);
  updateTopLayout->addStretch();

  QWidget *updatePanel =
      new GlassPanel(currentTheme == Dark, GlassPanel::Strong, this);
  updatePanel->setObjectName("panelWidget");
  QVBoxLayout *panelLayout = new QVBoxLayout(updatePanel);
  panelLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

  versionLabel = new QLabel(QStringLiteral(APP_VERSION), this);
  versionLabel->setAlignment(Qt::AlignCenter);
  versionLabel->setObjectName("versionLabel");
  statusLabel = new QLabel("", this);
  statusLabel->setAlignment(Qt::AlignCenter);
  statusLabel->setObjectName("statusLabel");
  statusLabel->setWordWrap(true);
  statusLabel->setMinimumWidth(400);
  updatePlanLabel = new QLabel("", this);
  updatePlanLabel->setAlignment(Qt::AlignCenter);
  updatePlanLabel->setObjectName("maintenancePlanLabel");
  updatePlanLabel->setWordWrap(true);
  updatePlanLabel->setMinimumWidth(420);
  updateRiskLabel = new QLabel("", this);
  updateRiskLabel->setAlignment(Qt::AlignCenter);
  updateRiskLabel->setObjectName("maintenanceRiskLabel");
  updateRiskLabel->setWordWrap(true);
  updateRiskLabel->setMinimumWidth(420);

  updateButton =
      new AnimatedButton(AnimatedButton::Primary, currentTheme == Dark, this);
  updateButton->setObjectName("systemUpdateButton");
  updateButton->setFixedSize(320, 70);
  updateButton->setCursor(Qt::PointingHandCursor);
  connect(updateButton, &QPushButton::clicked, this, &MainWindow::startUpdate);

  openRoControlButton =
      new AnimatedButton(AnimatedButton::Secondary, currentTheme == Dark, this);
  openRoControlButton->setObjectName("openRoControlButton");
  openRoControlButton->setMinimumSize(320, 46);
  openRoControlButton->setCursor(Qt::PointingHandCursor);
  connect(openRoControlButton, &QPushButton::clicked, this,
          &MainWindow::openRoControl);
  openRoControlButton->hide();

  progressBar = new QProgressBar(this);
  progressBar->setRange(0, 100);
  progressBar->setValue(0);
  progressBar->hide();
  progressBar->setFixedWidth(400);

  toggleLogBtn =
      new AnimatedButton(AnimatedButton::Secondary, currentTheme == Dark, this);
  toggleLogBtn->setObjectName("backButton");
  toggleLogBtn->setCursor(Qt::PointingHandCursor);
  connect(toggleLogBtn, &QPushButton::clicked, this,
          &MainWindow::toggleUpdateLogs);

  logConsole->setReadOnly(true);
  logConsole->setObjectName("logConsole");
  logConsole->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  logConsole->setMaximumHeight(150);
  logConsole->setMinimumHeight(100);

  panelLayout->addWidget(versionLabel, 0, Qt::AlignCenter);
  panelLayout->addSpacing(10);
  panelLayout->addWidget(statusLabel, 0, Qt::AlignCenter);
  panelLayout->addSpacing(10);
  panelLayout->addWidget(updatePlanLabel, 0, Qt::AlignCenter);
  panelLayout->addSpacing(10);
  panelLayout->addWidget(updateRiskLabel, 0, Qt::AlignCenter);
  panelLayout->addWidget(openRoControlButton, 0, Qt::AlignCenter);
  panelLayout->addSpacing(20);
  panelLayout->addWidget(updateButton, 0, Qt::AlignCenter);
  panelLayout->addSpacing(20);
  panelLayout->addWidget(progressBar, 0, Qt::AlignCenter);
  panelLayout->addSpacing(10);
  panelLayout->addWidget(toggleLogBtn, 0, Qt::AlignCenter);
  panelLayout->addSpacing(5);
  panelLayout->addWidget(logConsole, 1);

  updateLayout->addLayout(updateTopLayout);
  updateLayout->addSpacing(10);
  updateLayout->addWidget(updatePanel, 1);

  // 3. LIBRARY PACKAGE VIEW
  libraryViewWidget = new QWidget(this);
  QVBoxLayout *libraryLayout = new QVBoxLayout(libraryViewWidget);
  libraryLayout->setContentsMargins(20, 0, 20, 0);

  backFromLibraryBtn =
      new AnimatedButton(AnimatedButton::Secondary, currentTheme == Dark, this);
  backFromLibraryBtn->setObjectName("backButton");
  backFromLibraryBtn->setMinimumSize(120, 40);
  backFromLibraryBtn->setCursor(Qt::PointingHandCursor);
  connect(backFromLibraryBtn, &QPushButton::clicked, this,
          &MainWindow::showHomeScreen);

  QHBoxLayout *libraryTopLayout = new QHBoxLayout();
  libraryTopLayout->addWidget(backFromLibraryBtn);
  libraryTopLayout->addStretch();

  QWidget *libraryPanel =
      new GlassPanel(currentTheme == Dark, GlassPanel::Strong, this);
  libraryPanel->setObjectName("panelWidget");
  QVBoxLayout *libraryPanelLayout = new QVBoxLayout(libraryPanel);
  libraryPanelLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

  libraryStatusLabel = new QLabel("", this);
  libraryStatusLabel->setAlignment(Qt::AlignCenter);
  libraryStatusLabel->setObjectName("statusLabel");
  libraryStatusLabel->setWordWrap(true);
  libraryStatusLabel->setMinimumWidth(400);

  libraryInstallButton =
      new AnimatedButton(AnimatedButton::Primary, currentTheme == Dark, this);
  libraryInstallButton->setObjectName("libraryInstallButton");
  libraryInstallButton->setFixedSize(320, 70);
  libraryInstallButton->setCursor(Qt::PointingHandCursor);
  connect(libraryInstallButton, &QPushButton::clicked, this,
          &MainWindow::startLibraryPackageInstall);

  libraryProgressBar = new QProgressBar(this);
  libraryProgressBar->setRange(0, 100);
  libraryProgressBar->setValue(0);
  libraryProgressBar->hide();
  libraryProgressBar->setFixedWidth(400);

  toggleLibraryLogBtn =
      new AnimatedButton(AnimatedButton::Secondary, currentTheme == Dark, this);
  toggleLibraryLogBtn->setObjectName("backButton");
  toggleLibraryLogBtn->setCursor(Qt::PointingHandCursor);
  connect(toggleLibraryLogBtn, &QPushButton::clicked, this,
          &MainWindow::toggleLibraryLogs);

  libraryLogConsole->setReadOnly(true);
  libraryLogConsole->setObjectName("logConsole");
  libraryLogConsole->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  libraryLogConsole->setMaximumHeight(150);
  libraryLogConsole->setMinimumHeight(100);

  libraryPanelLayout->addSpacing(40);
  libraryPanelLayout->addWidget(libraryStatusLabel, 0, Qt::AlignCenter);
  libraryPanelLayout->addSpacing(20);
  libraryPanelLayout->addWidget(libraryInstallButton, 0, Qt::AlignCenter);
  libraryPanelLayout->addSpacing(20);
  libraryPanelLayout->addWidget(libraryProgressBar, 0, Qt::AlignCenter);
  libraryPanelLayout->addSpacing(10);
  libraryPanelLayout->addWidget(toggleLibraryLogBtn, 0, Qt::AlignCenter);
  libraryPanelLayout->addSpacing(5);
  libraryPanelLayout->addWidget(libraryLogConsole, 1);

  libraryLayout->addLayout(libraryTopLayout);
  libraryLayout->addSpacing(10);
  libraryLayout->addWidget(libraryPanel, 1);

  // 4. CUSTOM APP STORE VIEW
  appStoreViewWidget = new QWidget(this);
  QVBoxLayout *storeLayout = new QVBoxLayout(appStoreViewWidget);
  storeLayout->setContentsMargins(20, 0, 20, 0);

  backFromAppStoreBtn =
      new AnimatedButton(AnimatedButton::Secondary, currentTheme == Dark, this);
  backFromAppStoreBtn->setObjectName("backButton");
  backFromAppStoreBtn->setMinimumSize(120, 40);
  backFromAppStoreBtn->setCursor(Qt::PointingHandCursor);
  connect(backFromAppStoreBtn, &QPushButton::clicked, this,
          &MainWindow::showHomeScreen);

  QHBoxLayout *storeTopLayout = new QHBoxLayout();
  storeTopLayout->addWidget(backFromAppStoreBtn);
  storeTopLayout->addStretch();

  QWidget *storePanel =
      new GlassPanel(currentTheme == Dark, GlassPanel::Strong, this);
  storePanel->setObjectName("panelWidget");
  QVBoxLayout *storePanelLayout = new QVBoxLayout(storePanel);
  storePanelLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

  appStoreTitleLabel = new QLabel(this);
  appStoreTitleLabel->setObjectName("slideTitle");
  appStoreTitleLabel->setAlignment(Qt::AlignCenter);
  appStorePlaceholderIcon = new QLabel("🛍️", this);
  appStorePlaceholderIcon->setAlignment(Qt::AlignCenter);
  appStorePlaceholderIcon->setStyleSheet(
      "font-size: 150px; background-color: transparent; border: none;");
  appStoreOpenAppBtn =
      new AnimatedButton(AnimatedButton::Primary, currentTheme == Dark, this);
  appStoreOpenAppBtn->setObjectName("actionButton");
  appStoreOpenAppBtn->setMinimumSize(320, 70);
  appStoreOpenAppBtn->setCursor(Qt::PointingHandCursor);
  connect(appStoreOpenAppBtn, &QPushButton::clicked, this,
          &MainWindow::dummyAppStoreAction);

  storePanelLayout->addStretch();
  storePanelLayout->addWidget(appStoreTitleLabel, 0, Qt::AlignCenter);
  storePanelLayout->addSpacing(40);
  storePanelLayout->addWidget(appStorePlaceholderIcon, 0, Qt::AlignCenter);
  storePanelLayout->addSpacing(40);
  storePanelLayout->addWidget(appStoreOpenAppBtn, 0, Qt::AlignCenter);
  storePanelLayout->addStretch();

  storeLayout->addLayout(storeTopLayout);
  storeLayout->addSpacing(10);
  storeLayout->addWidget(storePanel, 1);
  createDashboard();

  // 5. SOCIAL LINKS VIEW
  socialViewWidget = new QWidget(this);
  QVBoxLayout *socialViewLayout = new QVBoxLayout(socialViewWidget);
  socialViewLayout->setContentsMargins(20, 0, 20, 0);
  backFromSocialBtn =
      new AnimatedButton(AnimatedButton::Secondary, currentTheme == Dark, this);
  backFromSocialBtn->setObjectName("backButton");
  backFromSocialBtn->setMinimumSize(120, 40);
  backFromSocialBtn->setCursor(Qt::PointingHandCursor);
  connect(backFromSocialBtn, &QPushButton::clicked, this,
          &MainWindow::showHomeScreen);
  QHBoxLayout *socialTopLayout = new QHBoxLayout();
  socialTopLayout->addWidget(backFromSocialBtn);
  socialTopLayout->addStretch();

  QWidget *socialPanel =
      new GlassPanel(currentTheme == Dark, GlassPanel::Strong, this);
  socialPanel->setObjectName("panelWidget");
  QVBoxLayout *socialPanelLayout = new QVBoxLayout(socialPanel);
  socialPanelLayout->setAlignment(Qt::AlignCenter);
  socialTitleLabel = new QLabel(this);
  socialTitleLabel->setObjectName("slideTitle");
  socialTitleLabel->setAlignment(Qt::AlignCenter);
  socialTitleLabel->setWordWrap(true);
  socialDescriptionLabel = new QLabel(this);
  socialDescriptionLabel->setObjectName("slideDesc");
  socialDescriptionLabel->setAlignment(Qt::AlignCenter);
  socialDescriptionLabel->setWordWrap(true);

  QHBoxLayout *socialLayout = new QHBoxLayout();
  socialLayout->setSpacing(20);
  websiteBtn = new QToolButton(this);
  roAsdGitHubBtn = new QToolButton(this);
  roAssistGitHubBtn = new QToolButton(this);
  for (QToolButton *button : {websiteBtn, roAsdGitHubBtn, roAssistGitHubBtn}) {
    button->setObjectName("squareSoftButton");
    button->setFixedSize(160, 160);
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setIconSize(QSize(64, 64));
    button->setCursor(Qt::PointingHandCursor);
  }
  websiteBtn->setIcon(QIcon(":/icons/globe.svg"));
  roAsdGitHubBtn->setIcon(QIcon(":/icons/github.svg"));
  roAssistGitHubBtn->setIcon(QIcon(":/icons/github.svg"));
  connect(websiteBtn, &QToolButton::clicked, this, &MainWindow::openWebsite);
  connect(roAsdGitHubBtn, &QToolButton::clicked, this,
          &MainWindow::openRoAsdGitHub);
  connect(roAssistGitHubBtn, &QToolButton::clicked, this,
          &MainWindow::openRoAssistGitHub);
  socialLayout->addStretch();
  socialLayout->addWidget(websiteBtn);
  socialLayout->addWidget(roAsdGitHubBtn);
  socialLayout->addWidget(roAssistGitHubBtn);
  socialLayout->addStretch();
  socialPanelLayout->addStretch();
  socialPanelLayout->addWidget(socialTitleLabel);
  socialPanelLayout->addSpacing(16);
  socialPanelLayout->addWidget(socialDescriptionLabel);
  socialPanelLayout->addSpacing(30);
  socialPanelLayout->addLayout(socialLayout);
  socialPanelLayout->addStretch();
  socialViewLayout->addLayout(socialTopLayout);
  socialViewLayout->addWidget(socialPanel, 1);

  // 6. COMMUNITY VIEW
  communityViewWidget = new QWidget(this);
  QVBoxLayout *communityLayout = new QVBoxLayout(communityViewWidget);
  communityLayout->setContentsMargins(20, 0, 20, 0);
  backFromCommunityBtn =
      new AnimatedButton(AnimatedButton::Secondary, currentTheme == Dark, this);
  backFromCommunityBtn->setObjectName("backButton");
  backFromCommunityBtn->setMinimumSize(120, 40);
  backFromCommunityBtn->setCursor(Qt::PointingHandCursor);
  connect(backFromCommunityBtn, &QPushButton::clicked, this,
          &MainWindow::showHomeScreen);
  QHBoxLayout *communityTopLayout = new QHBoxLayout();
  communityTopLayout->addWidget(backFromCommunityBtn);
  communityTopLayout->addStretch();

  QWidget *communityPanel =
      new GlassPanel(currentTheme == Dark, GlassPanel::Strong, this);
  communityPanel->setObjectName("panelWidget");
  QVBoxLayout *communityPanelLayout = new QVBoxLayout(communityPanel);
  communityPanelLayout->setAlignment(Qt::AlignCenter);
  communityTitleLabel = new QLabel(this);
  communityTitleLabel->setObjectName("slideTitle");
  communityTitleLabel->setAlignment(Qt::AlignCenter);
  communityTitleLabel->setWordWrap(true);
  communityDescriptionLabel = new QLabel(this);
  communityDescriptionLabel->setObjectName("slideDesc");
  communityDescriptionLabel->setAlignment(Qt::AlignCenter);
  communityDescriptionLabel->setWordWrap(true);
  bozokBtn =
      new AnimatedButton(AnimatedButton::Primary, currentTheme == Dark, this);
  bozokBtn->setObjectName("actionButton");
  bozokBtn->setMinimumSize(280, 70);
  bozokBtn->setCursor(Qt::PointingHandCursor);
  connect(bozokBtn, &QPushButton::clicked, this,
          &MainWindow::openBozokCommunity);
  communityPanelLayout->addStretch();
  communityPanelLayout->addWidget(communityTitleLabel);
  communityPanelLayout->addSpacing(20);
  communityPanelLayout->addWidget(communityDescriptionLabel);
  communityPanelLayout->addSpacing(40);
  communityPanelLayout->addWidget(bozokBtn, 0, Qt::AlignCenter);
  communityPanelLayout->addStretch();
  communityLayout->addLayout(communityTopLayout);
  communityLayout->addWidget(communityPanel, 1);

  // 7. PRINTER AND SCANNER SUPPORT VIEW
  printerSupportViewWidget = new QWidget(this);
  printerSupportViewWidget->setObjectName("printerSupportView");
  QVBoxLayout *printerLayout = new QVBoxLayout(printerSupportViewWidget);
  printerLayout->setContentsMargins(20, 0, 20, 0);
  backFromPrinterSupportBtn =
      new AnimatedButton(AnimatedButton::Secondary, currentTheme == Dark, this);
  backFromPrinterSupportBtn->setObjectName("backButton");
  backFromPrinterSupportBtn->setMinimumSize(120, 40);
  backFromPrinterSupportBtn->setCursor(Qt::PointingHandCursor);
  connect(backFromPrinterSupportBtn, &QPushButton::clicked, this,
          &MainWindow::showHomeScreen);
  QHBoxLayout *printerTopLayout = new QHBoxLayout();
  printerTopLayout->addWidget(backFromPrinterSupportBtn);
  printerTopLayout->addStretch();

  QWidget *printerPanel = new QWidget(this);
  printerPanel->setObjectName("panelWidget");
  QVBoxLayout *printerPanelLayout = new QVBoxLayout(printerPanel);
  printerPanelLayout->setContentsMargins(40, 24, 40, 30);
  printerPanelLayout->setSpacing(22);
  printerSupportTitleLabel = new QLabel(this);
  printerSupportTitleLabel->setObjectName("printerTitle");
  printerSupportTitleLabel->setAlignment(Qt::AlignLeft);
  printerSupportTitleLabel->setWordWrap(true);
  printerSupportDescriptionLabel = new QLabel(this);
  printerSupportDescriptionLabel->setObjectName("printerDescription");
  printerSupportDescriptionLabel->setAlignment(Qt::AlignLeft);
  printerSupportDescriptionLabel->setWordWrap(true);
  printerSupportBenefitsLabel = new QLabel(this);
  printerSupportBenefitsLabel->setObjectName("printerBenefits");
  printerSupportBenefitsLabel->setAlignment(Qt::AlignLeft);
  printerSupportBenefitsLabel->setWordWrap(true);
  printerSupportStatusLabel = new QLabel(this);
  printerSupportStatusLabel->setObjectName("printerStatus");
  printerSupportStatusLabel->setAlignment(Qt::AlignLeft);
  printerSupportStatusLabel->setWordWrap(true);

  printerSupportInstallButton =
      new AnimatedButton(AnimatedButton::Primary, currentTheme == Dark, this);
  printerSupportInstallButton->setObjectName("printerSupportInstallButton");
  printerSupportInstallButton->setMinimumSize(320, 62);
  printerSupportInstallButton->setCursor(Qt::PointingHandCursor);
  connect(printerSupportInstallButton, &QPushButton::clicked, this,
          &MainWindow::startPrinterSupportInstall);
  printerSupportLaterButton =
      new AnimatedButton(AnimatedButton::Secondary, currentTheme == Dark, this);
  printerSupportLaterButton->setObjectName("printerSupportLaterButton");
  printerSupportLaterButton->setMinimumSize(320, 46);
  printerSupportLaterButton->setCursor(Qt::PointingHandCursor);
  connect(printerSupportLaterButton, &QPushButton::clicked, this,
          &MainWindow::choosePrinterSupportLater);
  printerSupportDisableButton =
      new AnimatedButton(AnimatedButton::Secondary, currentTheme == Dark, this);
  printerSupportDisableButton->setObjectName("printerSupportDisableButton");
  printerSupportDisableButton->setMinimumSize(320, 46);
  printerSupportDisableButton->setCursor(Qt::PointingHandCursor);
  connect(printerSupportDisableButton, &QPushButton::clicked, this,
          &MainWindow::disablePrinterSupport);
  openPrinterSettingsButton =
      new AnimatedButton(AnimatedButton::Secondary, currentTheme == Dark, this);
  openPrinterSettingsButton->setObjectName("backButton");
  openPrinterSettingsButton->setCursor(Qt::PointingHandCursor);
  connect(openPrinterSettingsButton, &QPushButton::clicked, this,
          &MainWindow::openPrinterSettings);
  openScannerButton =
      new AnimatedButton(AnimatedButton::Secondary, currentTheme == Dark, this);
  openScannerButton->setObjectName("backButton");
  openScannerButton->setCursor(Qt::PointingHandCursor);
  connect(openScannerButton, &QPushButton::clicked, this,
          &MainWindow::openScannerApplication);

  printerSupportProgressBar = new QProgressBar(this);
  printerSupportProgressBar->setRange(0, 100);
  printerSupportProgressBar->setValue(0);
  printerSupportProgressBar->setFixedWidth(500);
  printerSupportProgressBar->hide();
  printerLogConsole->setReadOnly(true);
  printerLogConsole->setObjectName("logConsole");
  printerLogConsole->setMaximumHeight(130);
  printerLogConsole->hide();

  QWidget *printerInfoPanel =
      new GlassPanel(currentTheme == Dark, GlassPanel::Soft, printerPanel);
  printerInfoPanel->setObjectName("printerInfoPanel");
  QVBoxLayout *printerInfoLayout = new QVBoxLayout(printerInfoPanel);
  printerInfoLayout->setContentsMargins(32, 30, 32, 30);
  QLabel *printerIcon = new QLabel(QStringLiteral("🖨️"), printerInfoPanel);
  printerIcon->setObjectName("printerIllustration");
  printerIcon->setAlignment(Qt::AlignLeft);
  printerInfoLayout->addWidget(printerIcon);
  printerInfoLayout->addSpacing(10);
  printerInfoLayout->addWidget(printerSupportTitleLabel);
  printerInfoLayout->addSpacing(12);
  printerInfoLayout->addWidget(printerSupportDescriptionLabel);
  printerInfoLayout->addSpacing(24);
  printerInfoLayout->addWidget(printerSupportBenefitsLabel);
  printerInfoLayout->addStretch();

  QWidget *printerActionPanel =
      new GlassPanel(currentTheme == Dark, GlassPanel::Soft, printerPanel);
  printerActionPanel->setObjectName("printerActionPanel");
  QVBoxLayout *printerActionLayout = new QVBoxLayout(printerActionPanel);
  printerActionLayout->setContentsMargins(28, 30, 28, 30);
  printerActionLayout->addWidget(printerSupportStatusLabel);
  printerActionLayout->addSpacing(20);
  printerActionLayout->addWidget(printerSupportInstallButton);
  printerActionLayout->addSpacing(10);
  printerActionLayout->addWidget(printerSupportLaterButton);
  printerActionLayout->addSpacing(10);
  printerActionLayout->addWidget(printerSupportDisableButton);
  printerActionLayout->addSpacing(20);
  printerActionLayout->addWidget(printerSupportProgressBar);
  printerActionLayout->addWidget(printerLogConsole);
  printerActionLayout->addStretch();

  QHBoxLayout *printerContentLayout = new QHBoxLayout();
  printerContentLayout->setSpacing(24);
  printerContentLayout->addWidget(printerInfoPanel, 3);
  printerContentLayout->addWidget(printerActionPanel, 2);

  QWidget *printerToolsPanel =
      new GlassPanel(currentTheme == Dark, GlassPanel::Soft, printerPanel);
  printerToolsPanel->setObjectName("printerToolsPanel");
  QHBoxLayout *printerToolsLayout = new QHBoxLayout(printerToolsPanel);
  printerToolsLayout->setContentsMargins(24, 16, 24, 16);
  printerToolsLayout->addWidget(openPrinterSettingsButton);
  printerToolsLayout->addWidget(openScannerButton);
  printerPanelLayout->addLayout(printerContentLayout, 1);
  printerPanelLayout->addWidget(printerToolsPanel);
  printerLayout->addLayout(printerTopLayout);
  printerLayout->addWidget(
      makeScrollablePanel(printerSupportViewWidget, printerPanel,
                          QStringLiteral("printerSupportScrollArea")),
      1);

  // 8. PRIVACY AND TELEMETRY VIEW
  telemetryViewWidget = new QWidget(this);
  telemetryViewWidget->setObjectName("telemetryView");
  QVBoxLayout *telemetryLayout = new QVBoxLayout(telemetryViewWidget);
  telemetryLayout->setContentsMargins(20, 0, 20, 0);

  backFromTelemetryBtn =
      new AnimatedButton(AnimatedButton::Secondary, currentTheme == Dark, this);
  backFromTelemetryBtn->setObjectName("backButton");
  backFromTelemetryBtn->setMinimumSize(120, 40);
  backFromTelemetryBtn->setCursor(Qt::PointingHandCursor);
  connect(backFromTelemetryBtn, &QPushButton::clicked, this,
          &MainWindow::showHomeScreen);

  QHBoxLayout *telemetryTopLayout = new QHBoxLayout();
  telemetryTopLayout->addWidget(backFromTelemetryBtn);
  telemetryTopLayout->addStretch();

  QWidget *telemetryPanel = new QWidget(this);
  telemetryPanel->setObjectName("panelWidget");
  QVBoxLayout *telemetryPanelLayout = new QVBoxLayout(telemetryPanel);
  telemetryPanelLayout->setContentsMargins(40, 24, 40, 30);
  telemetryPanelLayout->setSpacing(22);

  telemetryTitleLabel = new QLabel(this);
  telemetryTitleLabel->setObjectName("telemetryTitle");
  telemetryTitleLabel->setAlignment(Qt::AlignLeft);
  telemetryTitleLabel->setWordWrap(true);
  telemetryIntroLabel = new QLabel(this);
  telemetryIntroLabel->setObjectName("telemetryBody");
  telemetryIntroLabel->setAlignment(Qt::AlignLeft);
  telemetryIntroLabel->setWordWrap(true);
  telemetryPurposeTitleLabel = new QLabel(this);
  telemetryPurposeTitleLabel->setObjectName("telemetrySectionTitle");
  telemetryPurposeTitleLabel->setAlignment(Qt::AlignLeft);
  telemetryPurposeTitleLabel->setWordWrap(true);
  telemetryPurposeTextLabel = new QLabel(this);
  telemetryPurposeTextLabel->setObjectName("telemetryBody");
  telemetryPurposeTextLabel->setAlignment(Qt::AlignLeft);
  telemetryPurposeTextLabel->setWordWrap(true);
  telemetryLevelsTitleLabel = new QLabel(this);
  telemetryLevelsTitleLabel->setObjectName("telemetrySectionTitle");
  telemetryLevelsTitleLabel->setAlignment(Qt::AlignLeft);
  telemetryLevelsTitleLabel->setWordWrap(true);
  telemetryLevelsTextLabel = new QLabel(this);
  telemetryLevelsTextLabel->setObjectName("telemetryList");
  telemetryLevelsTextLabel->setAlignment(Qt::AlignLeft);
  telemetryLevelsTextLabel->setWordWrap(true);
  telemetryDoesNotCollectTitleLabel = new QLabel(this);
  telemetryDoesNotCollectTitleLabel->setObjectName("telemetrySectionTitle");
  telemetryDoesNotCollectTitleLabel->setAlignment(Qt::AlignLeft);
  telemetryDoesNotCollectTitleLabel->setWordWrap(true);
  telemetryDoesNotCollectTextLabel = new QLabel(this);
  telemetryDoesNotCollectTextLabel->setObjectName("telemetryList");
  telemetryDoesNotCollectTextLabel->setAlignment(Qt::AlignLeft);
  telemetryDoesNotCollectTextLabel->setWordWrap(true);
  telemetrySliderTitleLabel = new QLabel(this);
  telemetrySliderTitleLabel->setObjectName("telemetrySectionTitle");
  telemetrySliderTitleLabel->setAlignment(Qt::AlignLeft);
  telemetrySliderTitleLabel->setWordWrap(true);
  telemetryCurrentLevelLabel = new QLabel(this);
  telemetryCurrentLevelLabel->setObjectName("telemetryCurrentLevelLabel");
  telemetryCurrentLevelLabel->setAlignment(Qt::AlignLeft);
  telemetryCurrentLevelLabel->setWordWrap(true);
  telemetryLevelDescriptionLabel = new QLabel(this);
  telemetryLevelDescriptionLabel->setObjectName("telemetryBody");
  telemetryLevelDescriptionLabel->setAlignment(Qt::AlignLeft);
  telemetryLevelDescriptionLabel->setWordWrap(true);

  QWidget *telemetryInfoPanel =
      new GlassPanel(currentTheme == Dark, GlassPanel::Soft, telemetryPanel);
  telemetryInfoPanel->setObjectName("telemetryInfoPanel");
  QVBoxLayout *telemetryInfoLayout = new QVBoxLayout(telemetryInfoPanel);
  telemetryInfoLayout->setContentsMargins(32, 30, 32, 30);
  QLabel *telemetryIcon = new QLabel(QStringLiteral("◌"), telemetryInfoPanel);
  telemetryIcon->setObjectName("telemetryIllustration");
  telemetryIcon->setAlignment(Qt::AlignLeft);
  telemetryInfoLayout->addWidget(telemetryIcon);
  telemetryInfoLayout->addSpacing(10);
  telemetryInfoLayout->addWidget(telemetryTitleLabel);
  telemetryInfoLayout->addSpacing(12);
  telemetryInfoLayout->addWidget(telemetryIntroLabel);
  telemetryInfoLayout->addSpacing(24);
  telemetryInfoLayout->addWidget(telemetryPurposeTitleLabel);
  telemetryInfoLayout->addSpacing(8);
  telemetryInfoLayout->addWidget(telemetryPurposeTextLabel);
  telemetryInfoLayout->addSpacing(18);
  telemetryInfoLayout->addWidget(telemetryDoesNotCollectTitleLabel);
  telemetryInfoLayout->addSpacing(8);
  telemetryInfoLayout->addWidget(telemetryDoesNotCollectTextLabel);
  telemetryInfoLayout->addStretch();

  QWidget *telemetryControlPanel =
      new GlassPanel(currentTheme == Dark, GlassPanel::Soft, telemetryPanel);
  telemetryControlPanel->setObjectName("telemetryControlPanel");
  QVBoxLayout *telemetryControlLayout = new QVBoxLayout(telemetryControlPanel);
  telemetryControlLayout->setContentsMargins(28, 30, 28, 30);

  telemetryLevelSlider = new QSlider(Qt::Horizontal, telemetryControlPanel);
  telemetryLevelSlider->setObjectName("telemetryLevelSlider");
  telemetryLevelSlider->setRange(0, 3);
  telemetryLevelSlider->setTickInterval(1);
  telemetryLevelSlider->setSingleStep(1);
  telemetryLevelSlider->setPageStep(1);
  telemetryLevelSlider->setTickPosition(QSlider::TicksBelow);
  telemetryLevelSlider->setCursor(Qt::PointingHandCursor);
  QSettings telemetrySettings(QSettings::IniFormat, QSettings::UserScope,
                              QStringLiteral("Project-Ro-ASD"),
                              QStringLiteral("ro-assist"));
  const int telemetryLevel =
      boundedTelemetryLevel(telemetrySettings
                                .value(QStringLiteral("telemetry/level"), 1)
                                .toInt());
  telemetryLevelSlider->setValue(telemetryLevel);
  telemetrySettings.setValue(QStringLiteral("telemetry/level"),
                             telemetryLevel);
  telemetrySettings.setValue(QStringLiteral("telemetry/levelName"),
                             telemetryLevelSettingName(telemetryLevel));
  telemetrySettings.sync();
  connect(telemetryLevelSlider, &QSlider::valueChanged, this,
          &MainWindow::setTelemetryLevel);

  QWidget *telemetryScaleWidget = new QWidget(telemetryControlPanel);
  QHBoxLayout *telemetryScaleLayout = new QHBoxLayout(telemetryScaleWidget);
  telemetryScaleLayout->setContentsMargins(0, 0, 0, 0);
  telemetryOffLabel = new QLabel(telemetryScaleWidget);
  telemetryCountLabel = new QLabel(telemetryScaleWidget);
  telemetryBasicLabel = new QLabel(telemetryScaleWidget);
  telemetryExtendedLabel = new QLabel(telemetryScaleWidget);
  for (QLabel *label :
       {telemetryOffLabel, telemetryCountLabel, telemetryBasicLabel,
        telemetryExtendedLabel}) {
    label->setObjectName("telemetryScaleLabel");
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);
    telemetryScaleLayout->addWidget(label, 1);
  }

  telemetryControlLayout->addWidget(telemetrySliderTitleLabel);
  telemetryControlLayout->addSpacing(16);
  telemetryControlLayout->addWidget(telemetryLevelSlider);
  telemetryControlLayout->addWidget(telemetryScaleWidget);
  telemetryControlLayout->addSpacing(22);
  telemetryControlLayout->addWidget(telemetryCurrentLevelLabel);
  telemetryControlLayout->addSpacing(8);
  telemetryControlLayout->addWidget(telemetryLevelDescriptionLabel);
  telemetryControlLayout->addSpacing(24);
  telemetryControlLayout->addWidget(telemetryLevelsTitleLabel);
  telemetryControlLayout->addSpacing(8);
  telemetryControlLayout->addWidget(telemetryLevelsTextLabel);
  telemetryControlLayout->addStretch();

  QHBoxLayout *telemetryContentLayout = new QHBoxLayout();
  telemetryContentLayout->setSpacing(24);
  telemetryContentLayout->addWidget(telemetryInfoPanel, 3);
  telemetryContentLayout->addWidget(telemetryControlPanel, 2);

  telemetryPanelLayout->addLayout(telemetryContentLayout, 1);
  telemetryLayout->addLayout(telemetryTopLayout);
  telemetryLayout->addWidget(
      makeScrollablePanel(telemetryViewWidget, telemetryPanel,
                          QStringLiteral("telemetryScrollArea")),
      1);

  mainStack->addWidget(welcomeViewWidget);
  mainStack->addWidget(dashboardViewWidget);
  mainStack->addWidget(updateViewWidget);
  mainStack->addWidget(libraryViewWidget);
  mainStack->addWidget(appStoreViewWidget);
  mainStack->addWidget(socialViewWidget);
  mainStack->addWidget(communityViewWidget);
  mainStack->addWidget(printerSupportViewWidget);
  mainStack->addWidget(telemetryViewWidget);

  mainLayout->addWidget(mainStack, 1);

  // BOTTOM BAR
  QWidget *bottomBarWidget = new QWidget(this);
  QHBoxLayout *aboutLayout = new QHBoxLayout(bottomBarWidget);
  aboutLayout->setContentsMargins(20, 10, 20, 20);

  QPushButton *aboutBtn =
      new AnimatedButton(AnimatedButton::Icon, currentTheme == Dark, this);
  aboutBtn->setText("i");
  aboutBtn->setObjectName("aboutButton");
  aboutBtn->setFixedSize(50, 50);
  aboutBtn->setCursor(Qt::PointingHandCursor);
  connect(aboutBtn, &QPushButton::clicked, this, &MainWindow::showAboutDialog);

  aboutLayout->addWidget(aboutBtn);
  aboutLayout->addStretch();
  mainLayout->addWidget(bottomBarWidget);
}

void MainWindow::createWelcomeSlides() {
  auto addSlide = [this](QLabel *&title, QLabel *&description,
                         const QString &illustration, const QString &target) {
    QWidget *slide =
        new GlassPanel(currentTheme == Dark, GlassPanel::Soft, welcomeStack);
    QVBoxLayout *layout = new QVBoxLayout(slide);
    layout->setContentsMargins(32, 20, 32, 20);

    QLabel *icon = new QLabel(illustration, slide);
    icon->setObjectName("welcomeIllustration");
    icon->setAlignment(Qt::AlignCenter);
    title = new QLabel(slide);
    title->setObjectName("slideTitle");
    title->setAlignment(Qt::AlignCenter);
    title->setWordWrap(true);
    description = new QLabel(slide);
    description->setObjectName("slideDesc");
    description->setAlignment(Qt::AlignCenter);
    description->setWordWrap(true);

    for (QLabel *clickTarget : {icon, title, description}) {
      clickTarget->setProperty("welcomeTarget", target);
      clickTarget->setCursor(Qt::PointingHandCursor);
      clickTarget->installEventFilter(this);
    }

    layout->addStretch();
    layout->addWidget(icon);
    layout->addSpacing(20);
    layout->addWidget(title);
    layout->addSpacing(16);
    layout->addWidget(description);
    layout->addStretch();
    welcomeStack->addWidget(slide);
  };

  addSlide(slide1Title, slide1Desc, QStringLiteral("↻"),
           QStringLiteral("update"));
  addSlide(slide2Title, slide2Desc, QStringLiteral("⌘"),
           QStringLiteral("social"));
  addSlide(slide3Title, slide3Desc, QStringLiteral("▣"),
           QStringLiteral("store"));
  addSlide(slide4Title, slide4Desc, QStringLiteral("♥"),
           QStringLiteral("community"));
  addSlide(slide5Title, slide5Desc, QStringLiteral("◈"),
           QStringLiteral("library"));
  addSlide(slide6Title, slide6Desc, QStringLiteral("◌"),
           QStringLiteral("telemetry"));
}

void MainWindow::createDashboard() {
  dashboardViewWidget = new QWidget(this);
  QVBoxLayout *pageLayout = new QVBoxLayout(dashboardViewWidget);
  pageLayout->setContentsMargins(0, 0, 0, 0);

  auto *dashboardContent = new QWidget(dashboardViewWidget);
  dashboardContent->setObjectName("dashboardScrollContent");
  QVBoxLayout *layout = new QVBoxLayout(dashboardContent);
  layout->setContentsMargins(36, 20, 36, 30);
  layout->setSpacing(20);

  QWidget *profilePanel =
      new GlassPanel(currentTheme == Dark, GlassPanel::Soft,
                     dashboardContent);
  profilePanel->setObjectName("profilePanel");
  QHBoxLayout *profileLayout = new QHBoxLayout(profilePanel);
  profileLayout->setContentsMargins(24, 18, 24, 18);
  QLabel *avatar = new QLabel(profilePanel);
  avatar->setObjectName("profileAvatar");
  avatar->setAlignment(Qt::AlignCenter);
  avatar->setFixedSize(64, 64);
  const QPixmap logo(":/icons/ro-asd-logo.svg");
  avatar->setPixmap(logo.scaled(50, 50, Qt::KeepAspectRatio,
                                Qt::SmoothTransformation));
  dashboardGreetingLabel = new QLabel(profilePanel);
  dashboardGreetingLabel->setObjectName("dashboardGreeting");
  dashboardDescriptionLabel = new QLabel(profilePanel);
  dashboardDescriptionLabel->setObjectName("dashboardDescription");
  dashboardDescriptionLabel->setWordWrap(true);
  dashboardStatusLabel = new QLabel(profilePanel);
  dashboardStatusLabel->setObjectName("dashboardStatusLabel");
  dashboardStatusLabel->setWordWrap(true);
  QVBoxLayout *profileTextLayout = new QVBoxLayout();
  profileTextLayout->addWidget(dashboardGreetingLabel);
  profileTextLayout->addWidget(dashboardDescriptionLabel);
  profileTextLayout->addWidget(dashboardStatusLabel);
  profileLayout->addWidget(avatar);
  profileLayout->addSpacing(14);
  profileLayout->addLayout(profileTextLayout, 1);

  QGridLayout *grid = new QGridLayout();
  grid->setHorizontalSpacing(18);
  grid->setVerticalSpacing(18);
  grid->setColumnStretch(0, 1);
  grid->setColumnStretch(1, 1);

  auto configureCard = [this, dashboardContent](QPushButton *&card) {
    card =
        new AnimatedButton(AnimatedButton::Card, currentTheme == Dark,
                           dashboardContent);
    card->setObjectName("dashboardCard");
    card->setMinimumHeight(104);
    card->setCursor(Qt::PointingHandCursor);
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  };
  configureCard(dashboardUpdateCard);
  configureCard(dashboardSocialCard);
  configureCard(dashboardStoreCard);
  configureCard(dashboardCommunityCard);
  configureCard(dashboardLibraryCard);
  configureCard(dashboardPrinterCard);
  configureCard(dashboardTelemetryCard);
  dashboardUpdateCard->setObjectName("dashboardUpdateCard");
  dashboardSocialCard->setObjectName("dashboardSocialCard");
  dashboardStoreCard->setObjectName("dashboardStoreCard");
  dashboardCommunityCard->setObjectName("dashboardCommunityCard");
  dashboardLibraryCard->setObjectName("dashboardLibraryCard");
  dashboardPrinterCard->setObjectName("dashboardPrinterCard");
  dashboardTelemetryCard->setObjectName("dashboardTelemetryCard");
  dashboardUpdateCard->setProperty("cardIcon", QStringLiteral("update"));
  dashboardSocialCard->setProperty("cardIcon", QStringLiteral("social"));
  dashboardStoreCard->setProperty("cardIcon", QStringLiteral("store"));
  dashboardCommunityCard->setProperty("cardIcon", QStringLiteral("community"));
  dashboardLibraryCard->setProperty("cardIcon", QStringLiteral("library"));
  dashboardPrinterCard->setProperty("cardIcon", QStringLiteral("printer"));
  dashboardTelemetryCard->setProperty("cardIcon", QStringLiteral("telemetry"));
  connect(dashboardUpdateCard, &QPushButton::clicked, this,
          &MainWindow::showUpdateScreen);
  connect(dashboardSocialCard, &QPushButton::clicked, this,
          &MainWindow::showSocialScreen);
  connect(dashboardStoreCard, &QPushButton::clicked, this,
          &MainWindow::showAppStoreScreen);
  connect(dashboardCommunityCard, &QPushButton::clicked, this,
          &MainWindow::showCommunityScreen);
  connect(dashboardLibraryCard, &QPushButton::clicked, this,
          &MainWindow::showLibraryScreen);
  connect(dashboardPrinterCard, &QPushButton::clicked, this,
          &MainWindow::showPrinterSupportScreen);
  connect(dashboardTelemetryCard, &QPushButton::clicked, this,
          &MainWindow::showTelemetryScreen);

  grid->addWidget(dashboardUpdateCard, 0, 0);
  grid->addWidget(dashboardSocialCard, 0, 1);
  grid->addWidget(dashboardStoreCard, 1, 0);
  grid->addWidget(dashboardCommunityCard, 1, 1);
  grid->addWidget(dashboardPrinterCard, 2, 0);
  grid->addWidget(dashboardLibraryCard, 2, 1);
  grid->addWidget(dashboardTelemetryCard, 3, 0, 1, 2);

  layout->addWidget(profilePanel);
  layout->addLayout(grid);
  layout->addStretch();
  pageLayout->addWidget(makeScrollablePanel(
      dashboardViewWidget, dashboardContent,
      QStringLiteral("dashboardScrollArea")));
}

void MainWindow::updateUiTextAndImages() {
  const auto bundle = RoAssist::UiTexts::buildBundle(
      currentLanguageCode(), currentTheme == Dark, logConsole->isVisible(),
      libraryLogConsole->isVisible(), isLibraryInstalled);

  langBtn->setText(bundle.languageButton);
  themeToggleBtn->setText(bundle.themeToggle);
  networkStatusLabel->setText(bundle.networkStatus);
  backToCarouselBtn->setText(bundle.backButton);
  backFromLibraryBtn->setText(bundle.backButton);
  backFromAppStoreBtn->setText(bundle.backButton);
  backFromSocialBtn->setText(bundle.backButton);
  backFromCommunityBtn->setText(bundle.backButton);
  backFromPrinterSupportBtn->setText(bundle.backButton);
  backFromTelemetryBtn->setText(bundle.backButton);
  toggleLogBtn->setText(bundle.toggleLogs);
  toggleLibraryLogBtn->setText(bundle.toggleLibraryLogs);
  versionLabel->setText(RoAssist::UiTexts::versionText(
      currentLanguageCode(), QStringLiteral(APP_VERSION)));
  updateButton->setText(bundle.updateButton);
  welcomeProgressLabel->setText(
      bundle.welcomeProgress.arg(welcomeStack->currentIndex() + 1)
          .arg(welcomeStack->count()));
  welcomeNextBtn->setText(welcomeStack->currentIndex() == welcomeStack->count() - 1
                              ? bundle.welcomeFinish
                              : bundle.welcomeNext);
  dashboardGreetingLabel->setText(bundle.dashboardGreeting);
  dashboardDescriptionLabel->setText(bundle.dashboardDescription);
  dashboardUpdateCard->setText(bundle.dashboardUpdateCard);
  dashboardSocialCard->setText(bundle.dashboardSocialCard);
  dashboardStoreCard->setText(bundle.dashboardStoreCard);
  dashboardCommunityCard->setText(bundle.dashboardCommunityCard);
  dashboardLibraryCard->setText(bundle.dashboardLibraryCard);
  dashboardPrinterCard->setText(bundle.dashboardPrinterCard);
  dashboardTelemetryCard->setText(bundle.dashboardTelemetryCard);
  slide1Title->setText(bundle.slide1Title);
  slide1Desc->setText(bundle.slide1Description);
  slide2Title->setText(bundle.slide2Title);
  slide2Desc->setText(bundle.slide2Description);
  slide3Title->setText(bundle.slide3Title);
  slide3Desc->setText(bundle.slide3Description);
  appStoreTitleLabel->setText(bundle.appStoreTitle);
  appStoreOpenAppBtn->setText(bundle.appStoreOpenButton);
  slide4Title->setText(bundle.slide4Title);
  slide4Desc->setText(bundle.slide4Description);
  socialTitleLabel->setText(bundle.slide2Title);
  socialDescriptionLabel->setText(bundle.slide2Description);
  communityTitleLabel->setText(bundle.slide4Title);
  communityDescriptionLabel->setText(bundle.slide4Description);
  bozokBtn->setText(bundle.communityButton);
  slide5Title->setText(bundle.slide5Title);
  slide5Desc->setText(bundle.slide5Description);
  slide6Title->setText(bundle.slide6Title);
  slide6Desc->setText(bundle.slide6Description);
  if (activeOperation != LibraryInstall) {
    libraryStatusLabel->setText(bundle.libraryStatusIdle);
  }
  logConsole->setPlaceholderText(bundle.logPlaceholder);
  libraryLogConsole->setPlaceholderText(bundle.logPlaceholder);
  roAsdGitHubBtn->setText("ro-ASD OS\nRepo");
  roAssistGitHubBtn->setText("ro-Assist\nRepo");
  websiteBtn->setText(bundle.websiteButton);
  printerSupportTitleLabel->setText(bundle.printerSupportTitle);
  printerSupportDescriptionLabel->setText(bundle.printerSupportDescription);
  printerSupportBenefitsLabel->setText(bundle.printerSupportBenefits);
  printerSupportInstallButton->setText(bundle.printerSupportInstallButton);
  printerSupportLaterButton->setText(bundle.printerSupportLaterButton);
  printerSupportDisableButton->setText(bundle.printerSupportDisableButton);
  openPrinterSettingsButton->setText(bundle.printerSupportOpenSettingsButton);
  openScannerButton->setText(bundle.printerSupportOpenScannerButton);
  printerLogConsole->setPlaceholderText(bundle.logPlaceholder);
  telemetryTitleLabel->setText(bundle.telemetryTitle);
  telemetryIntroLabel->setText(bundle.telemetryIntro);
  telemetryPurposeTitleLabel->setText(bundle.telemetryPurposeTitle);
  telemetryPurposeTextLabel->setText(bundle.telemetryPurposeText);
  telemetryLevelsTitleLabel->setText(bundle.telemetryLevelsTitle);
  telemetryLevelsTextLabel->setText(bundle.telemetryLevelsText);
  telemetryDoesNotCollectTitleLabel->setText(
      bundle.telemetryDoesNotCollectTitle);
  telemetryDoesNotCollectTextLabel->setText(
      bundle.telemetryDoesNotCollectText);
  telemetrySliderTitleLabel->setText(bundle.telemetrySliderTitle);
  telemetryOffLabel->setText(bundle.telemetryLevelOff);
  telemetryCountLabel->setText(bundle.telemetryLevelCount);
  telemetryBasicLabel->setText(bundle.telemetryLevelBasic);
  telemetryExtendedLabel->setText(bundle.telemetryLevelExtended);
  QString telemetryLevelName;
  QString telemetryLevelDescription;
  switch (boundedTelemetryLevel(telemetryLevelSlider->value())) {
  case 0:
    telemetryLevelName = bundle.telemetryLevelOff;
    telemetryLevelDescription = bundle.telemetryDescriptionOff;
    break;
  case 2:
    telemetryLevelName = bundle.telemetryLevelBasic;
    telemetryLevelDescription = bundle.telemetryDescriptionBasic;
    break;
  case 3:
    telemetryLevelName = bundle.telemetryLevelExtended;
    telemetryLevelDescription = bundle.telemetryDescriptionExtended;
    break;
  case 1:
  default:
    telemetryLevelName = bundle.telemetryLevelCount;
    telemetryLevelDescription = bundle.telemetryDescriptionCount;
    break;
  }
  telemetryCurrentLevelLabel->setText(
      bundle.telemetryCurrentLevel.arg(telemetryLevelName));
  telemetryLevelDescriptionLabel->setText(telemetryLevelDescription);
  if (!isPrinterSupportOperationActive()) {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QStringLiteral("Project-Ro-ASD"),
                       QStringLiteral("ro-assist"));
    const QString preference =
        settings.value(QStringLiteral("printer/supportPreference")).toString();
    printerSupportStatusLabel->setText(
        isPrinterSupportInstalled || preference == QStringLiteral("installed")
            ? bundle.printerSupportInstalled
            : (preference == QStringLiteral("later")
                   ? bundle.printerSupportLaterSelected
                   : (preference == QStringLiteral("disabled")
                          ? bundle.printerSupportDisabled
                          : bundle.printerSupportStatusIdle)));
  }
  if (activeOperation != LibraryInstall &&
      checkLibProcess->state() == QProcess::NotRunning) {
    libraryInstallButton->setText(bundle.libraryActionButton);
  }

  refreshMaintenanceStatus();
}

void MainWindow::refreshMaintenanceStatus() {
  if (!updatePlanLabel || !updateRiskLabel || !openRoControlButton)
    return;

  if (!riskSnapshotReady && !riskWatcher->isRunning())
    riskWatcher->setFuture(QtConcurrent::run(&RoAssist::SystemRiskService::collect));

  applyMaintenanceStatus();
}

void MainWindow::applyMaintenanceStatus() {
  if (!updatePlanLabel || !updateRiskLabel || !openRoControlButton)
    return;

  const bool flatpakAvailable =
      RoAssist::SystemUpdateService::commandExists(QStringLiteral("flatpak"));
  const bool snapAvailable =
      RoAssist::SystemUpdateService::commandExists(QStringLiteral("snap"));

  updatePlanLabel->setText(RoAssist::UiTexts::maintenancePlanSummary(
      currentLanguageCode(), flatpakAvailable, snapAvailable));

  QStringList warningLines;
  if (RoAssist::SystemRiskService::isHighRiskGraphicsState(lastRiskSnapshot)) {
    warningLines.append(RoAssist::UiTexts::nouveauRiskWarning(
        currentLanguageCode(), lastRiskSnapshot.roControlAvailable));
  }
  if (lastRiskSnapshot.lowDiskSpace)
    warningLines.append(
        RoAssist::UiTexts::lowDiskWarning(currentLanguageCode()));
  if (lastRiskSnapshot.rebootRequired)
    warningLines.append(
        RoAssist::UiTexts::rebootRequiredWarning(currentLanguageCode()));
  if (warningLines.isEmpty())
    warningLines.append(
        RoAssist::UiTexts::maintenanceNoKnownRisk(currentLanguageCode()));

  const QString warningText = warningLines.join(QStringLiteral("\n"));
  updateRiskLabel->setText(warningText);
  if (dashboardStatusLabel)
    dashboardStatusLabel->setText(warningText);

  const bool graphicsRisk =
      RoAssist::SystemRiskService::isHighRiskGraphicsState(lastRiskSnapshot);
  openRoControlButton->setVisible(graphicsRisk);
  openRoControlButton->setEnabled(lastRiskSnapshot.roControlAvailable);
  openRoControlButton->setText(RoAssist::UiTexts::roControlButtonText(
      currentLanguageCode(), lastRiskSnapshot.roControlAvailable));

  if (activeOperation == None && updateProcess->state() == QProcess::NotRunning)
    setInitialUpdateStatus();

  if (activeOperation == None && updateButton)
    updateButton->setEnabled(isNetworkConnected &&
                             checkUpdateProcess->state() == QProcess::NotRunning &&
                             riskSnapshotReady);
}

void MainWindow::setInitialUpdateStatus() {
  if (updateProcess->state() != QProcess::NotRunning || transactionPhaseStarted)
    return;

  statusLabel->setText(checkUpdateProcess->state() != QProcess::NotRunning
                           ? RoAssist::UiTexts::buildBundle(
                                 currentLanguageCode(), currentTheme == Dark,
                                 logConsole->isVisible(),
                                 libraryLogConsole->isVisible(),
                                 isLibraryInstalled)
                                 .checkingUpdates
                           : RoAssist::UiTexts::maintenanceReady(
                                 currentLanguageCode()));
}

void MainWindow::setupStyle() {
  const bool dark = currentTheme == Dark;
  const ThemeTokens t = tokensForTheme(dark);
  if (auto *background = dynamic_cast<GradientBackground *>(centralWidget()))
    background->setDarkTheme(dark);
  for (QWidget *widget : findChildren<QWidget *>()) {
    if (auto *panel = dynamic_cast<GlassPanel *>(widget))
      panel->setDarkTheme(dark);
    if (auto *button = dynamic_cast<AnimatedButton *>(widget))
      button->setDarkTheme(dark);
  }

  const QString textCol = cssColor(t.text);
  const QString subTextCol = cssColor(t.muted);
  const QString borderCol = cssColor(t.border);
  const QString borderHot = cssColor(t.borderHot);
  const QString accent = cssColor(t.accent);
  const QString accentTwo = cssColor(t.accentTwo);
  const QString accentThree = cssColor(t.accentThree);
  const QString glassField =
      cssColor(dark ? QColor(18, 20, 24, 178) : QColor(255, 255, 255, 178));
  const QString glassFieldStrong =
      cssColor(dark ? QColor(12, 14, 18, 218) : QColor(255, 255, 255, 228));
  const QString disabledFill =
      cssColor(dark ? QColor(255, 255, 255, 28) : QColor(0, 0, 0, 22));

  QString style =
      QString(R"(
        * { outline: none; }
        QMainWindow { background: transparent; }
        QWidget { background: transparent; }
        QWidget#panelWidget { 
            background-color: transparent; 
            border: none; 
        }
        QLabel { color: %1; background: transparent; }
        QLabel#networkStatusLabel { color: #fff45f; font-size: 14px; font-weight: 800; }
        QLabel#versionLabel { font-size: 14px; font-weight: 700; color: %2; }
        QLabel#statusLabel { font-size: 16px; color: %2; font-weight: 700; }
        QLabel#maintenancePlanLabel { font-size: 14px; color: %2; font-weight: 700; }
        QLabel#maintenanceRiskLabel {
            background: %9; border: 1px solid %3; border-radius: 14px;
            color: %1; font-size: 14px; font-weight: 800; padding: 12px;
        }
        QLabel#slideTitle { font-size: 36px; font-weight: 900; margin-bottom: 8px; color: %1; }
        QLabel#slideDesc { font-size: 18px; color: %2; line-height: 1.5; }
        QLabel#welcomeIllustration { font-size: 92px; color: %6; }
        QLabel#welcomeProgressLabel { font-size: 14px; color: %2; font-weight: 800; }
        QLabel#profileAvatar {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 %5, stop:1 %6);
            color: white; border-radius: 32px; font-size: 28px; font-weight: 900;
        }
        QLabel#dashboardGreeting { font-size: 26px; font-weight: 900; color: %1; }
        QLabel#dashboardDescription { font-size: 15px; color: %2; }
        QLabel#dashboardStatusLabel { font-size: 14px; color: %1; font-weight: 800; }
        QLabel#printerIllustration { font-size: 58px; }
        QLabel#printerTitle { font-size: 34px; font-weight: 900; color: %1; }
        QLabel#printerDescription { font-size: 18px; color: %2; line-height: 1.45; }
        QLabel#printerBenefits {
            background: %9; border: 1px solid %3; border-radius: 16px;
            color: %1; font-size: 17px; line-height: 1.55; padding: 18px;
        }
        QLabel#printerStatus { font-size: 17px; color: %1; font-weight: 800; }
        QLabel#telemetryIllustration { font-size: 58px; color: %6; }
        QLabel#telemetryTitle { font-size: 36px; font-weight: 900; color: %1; }
        QLabel#telemetrySectionTitle { font-size: 18px; color: %1; font-weight: 900; }
        QLabel#telemetryBody { font-size: 16px; color: %2; line-height: 1.45; }
        QLabel#telemetryList {
            background: %9; border: 1px solid %3; border-radius: 16px;
            color: %1; font-size: 15px; line-height: 1.55; padding: 16px;
        }
        QLabel#telemetryCurrentLevelLabel { font-size: 18px; color: %1; font-weight: 900; }
        QLabel#telemetryScaleLabel { font-size: 13px; color: %2; font-weight: 800; }
        
        QTextEdit#logConsole {
            background: %9; color: %1; border: 1px solid %3;
            border-radius: 14px; font-family: 'Cascadia Code', 'Consolas', monospace; font-size: 12px;
            padding: 8px;
        }
        QPushButton, QToolButton {
            padding: 8px 16px; font-size: 14px; font-weight: 600;
        }
        QPushButton {
            background: transparent; border: none; color: %1;
            font-weight: 800;
        }
        QPushButton#systemUpdateButton, QPushButton#libraryInstallButton,
        QPushButton#actionButton, QPushButton#welcomeNextButton,
        QPushButton#printerSupportInstallButton {
            color: white; font-size: 18px; font-weight: 900; padding: 12px;
        }
        QPushButton#backButton, QPushButton#backToHomeButton,
        QPushButton#printerSupportLaterButton, QPushButton#printerSupportDisableButton,
        QPushButton#themeButton, QPushButton#languageButton,
        QPushButton#openRoControlButton {
            font-size: 14px; font-weight: 900;
        }
        QPushButton#dashboardUpdateCard, QPushButton#dashboardSocialCard,
        QPushButton#dashboardStoreCard, QPushButton#dashboardCommunityCard,
        QPushButton#dashboardLibraryCard, QPushButton#dashboardPrinterCard,
        QPushButton#dashboardTelemetryCard {
            background: transparent; border: none;
            color: %1; font-size: 17px; font-weight: 900; padding: 20px;
            text-align: left;
        }
        QSlider#telemetryLevelSlider::groove:horizontal {
            border: 1px solid %3; height: 14px; background: %8; border-radius: 7px;
        }
        QSlider#telemetryLevelSlider::handle:horizontal {
            background: qradialgradient(cx:0.35, cy:0.3, radius:0.8, fx:0.25, fy:0.2,
                                        stop:0 white, stop:0.35 %6, stop:1 %5);
            border: 2px solid %4; width: 30px; height: 30px;
            margin: -10px 0; border-radius: 15px;
        }
        QSlider#telemetryLevelSlider::sub-page:horizontal {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                        stop:0 %7, stop:0.5 %6, stop:1 %5);
            border-radius: 7px;
        }
        
        QToolButton#squareSoftButton { 
            border-radius: 20px; font-size: 15px; font-weight: 800;
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                        stop:0 %7, stop:0.5 %6, stop:1 %5);
            border: 1px solid %4; color: white; padding: 12px;
        }
        QToolButton#squareSoftButton:hover { 
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                        stop:0 %5, stop:0.5 %6, stop:1 %7);
            color: white; 
        }
        
        QPushButton#aboutButton { font-size: 22px; font-weight: 900; }
        
        QProgressBar { 
            border: none; border-radius: 8px; text-align: center; 
            background-color: %8; color: transparent; font-weight: bold; height: 16px;
        }
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                        stop:0 %7, stop:0.5 %6, stop:1 %5);
            border-radius: 8px;
        }
        QMenu {
            background: %10; color: %1; border: 1px solid %3; border-radius: 12px;
            padding: 8px;
        }
        QMenu::item { padding: 8px 18px; border-radius: 8px; }
        QMenu::item:selected {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 %7, stop:1 %6);
            color: white;
        }
    )")
          .arg(textCol, subTextCol, borderCol, borderHot, accent, accentTwo,
               accentThree, disabledFill, glassField, glassFieldStrong);

  setStyleSheet(style);
}

void MainWindow::advanceWelcome() {
  const int nextIndex = welcomeStack->currentIndex() + 1;
  if (nextIndex < welcomeStack->count()) {
    revealStackWidget(welcomeStack, welcomeStack->widget(nextIndex), true);
    updateUiTextAndImages();
    return;
  }

  QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                     QStringLiteral("Project-Ro-ASD"),
                     QStringLiteral("ro-assist"));
  settings.setValue(QStringLiteral("welcome/completed"), true);
  settings.sync();
  welcomeCompleted = true;
  showDashboardScreen();
}

void MainWindow::showWelcomeScreen() {
  revealStackWidget(mainStack, welcomeViewWidget, false);
  updateUiTextAndImages();
}

void MainWindow::showDashboardScreen() {
  revealStackWidget(mainStack, dashboardViewWidget, true);
}

void MainWindow::showHomeScreen() {
  if (welcomeCompleted)
    showDashboardScreen();
  else
    showWelcomeScreen();
}

void MainWindow::showUpdateScreen() {
  refreshMaintenanceStatus();
  revealStackWidget(mainStack, updateViewWidget, true);
}
void MainWindow::showLibraryScreen() {
  revealStackWidget(mainStack, libraryViewWidget, true);

  libraryInstallButton->setEnabled(false);
  libraryInstallButton->setText(
      RoAssist::UiTexts::checkingLibraries(currentLanguageCode()));

  if (qEnvironmentVariableIsEmpty("RO_ASSIST_SKIP_SYSTEM_CHECKS") &&
      checkLibProcess->state() == QProcess::NotRunning) {
    checkLibProcess->start("rpm", QStringList()
                                      << "-q" << "gamemode" << "mangohud"
                                      << "vulkan-loader" << "vulkan-tools");
  } else if (!qEnvironmentVariableIsEmpty("RO_ASSIST_SKIP_SYSTEM_CHECKS")) {
    libraryInstallButton->setEnabled(true);
    libraryInstallButton->setText(RoAssist::UiTexts::buildBundle(
                                      currentLanguageCode(),
                                      currentTheme == Dark,
                                      logConsole->isVisible(),
                                      libraryLogConsole->isVisible(),
                                      isLibraryInstalled)
                                      .libraryActionButton);
  }
}
void MainWindow::showAppStoreScreen() {
  revealStackWidget(mainStack, appStoreViewWidget, true);
}
void MainWindow::showSocialScreen() {
  revealStackWidget(mainStack, socialViewWidget, true);
}
void MainWindow::showCommunityScreen() {
  revealStackWidget(mainStack, communityViewWidget, true);
}
void MainWindow::showPrinterSupportScreen() {
  revealStackWidget(mainStack, printerSupportViewWidget, true);
  updateUiTextAndImages();
}
void MainWindow::showTelemetryScreen() {
  revealStackWidget(mainStack, telemetryViewWidget, true);
  updateUiTextAndImages();
}

void MainWindow::setTelemetryLevel(int level) {
  const int boundedLevel = boundedTelemetryLevel(level);
  if (telemetryLevelSlider->value() != boundedLevel) {
    telemetryLevelSlider->setValue(boundedLevel);
    return;
  }
  QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                     QStringLiteral("Project-Ro-ASD"),
                     QStringLiteral("ro-assist"));
  settings.setValue(QStringLiteral("telemetry/level"), boundedLevel);
  settings.setValue(QStringLiteral("telemetry/levelName"),
                    telemetryLevelSettingName(boundedLevel));
  settings.sync();
  updateUiTextAndImages();
}

void MainWindow::changeEvent(QEvent *event) {
  QMainWindow::changeEvent(event);
  if (event->type() == QEvent::LocaleChange)
    applySystemLanguage();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
  if (!welcomeCompleted && mainStack->currentWidget() == welcomeViewWidget &&
      event->type() == QEvent::MouseButtonRelease) {
    const QString target = watched->property("welcomeTarget").toString();
    if (target == QStringLiteral("update"))
      showUpdateScreen();
    else if (target == QStringLiteral("social"))
      showSocialScreen();
    else if (target == QStringLiteral("store"))
      showAppStoreScreen();
    else if (target == QStringLiteral("community"))
      showCommunityScreen();
    else if (target == QStringLiteral("library"))
      showLibraryScreen();
    else if (target == QStringLiteral("telemetry"))
      showTelemetryScreen();
    else
      return QMainWindow::eventFilter(watched, event);
    return true;
  }

  return QMainWindow::eventFilter(watched, event);
}

void MainWindow::changeLanguageAction(QAction *action) {
  if (!action)
    return;
  currentLang = static_cast<Language>(action->data().toInt());
  updateUiTextAndImages();
}

void MainWindow::changeLanguage(QAction *action) {
  if (!action)
    return;

  const QString languageCode = action->data().toString().toLower();
  if (languageCode == QStringLiteral("tr"))
    currentLang = TR;
  else if (languageCode == QStringLiteral("es"))
    currentLang = ES;
  else if (languageCode == QStringLiteral("de"))
    currentLang = DE;
  else if (languageCode == QStringLiteral("fr"))
    currentLang = FR;
  else
    currentLang = EN;
  updateUiTextAndImages();
}

void MainWindow::toggleTheme() {
  currentTheme = (currentTheme == Light) ? Dark : Light;
  updateUiTextAndImages();
  setupStyle();
}

void MainWindow::toggleUpdateLogs() {
  logConsole->setVisible(!logConsole->isVisible());
  updateUiTextAndImages();
}

void MainWindow::toggleLibraryLogs() {
  libraryLogConsole->setVisible(!libraryLogConsole->isVisible());
  updateUiTextAndImages();
}

void MainWindow::onNetworkConnectedChanged(bool isConnected) {
  isNetworkConnected = isConnected;
  networkStatusLabel->setVisible(!isNetworkConnected);
  refreshMaintenanceStatus();
}

void MainWindow::openWebsite() {
  QDesktopServices::openUrl(QUrl("https://github.com/Project-Ro-ASD"));
}
void MainWindow::openRoAsdGitHub() {
  QDesktopServices::openUrl(QUrl("https://github.com/Project-Ro-ASD/ro-asd"));
}
void MainWindow::openRoAssistGitHub() {
  QDesktopServices::openUrl(
      QUrl("https://github.com/Project-Ro-ASD/ro-Assist"));
}
void MainWindow::openBozokCommunity() {
  QDesktopServices::openUrl(QUrl("https://github.com/Project-Ro-ASD"));
}
void MainWindow::showAboutDialog() {
  const ThemeTokens tokens = tokensForTheme(currentTheme == Dark);
  QDialog dialog(this);
  dialog.setWindowTitle(
      RoAssist::UiTexts::aboutTitle(currentLanguageCode()));
  dialog.setMinimumSize(420, 320);
  dialog.resize(std::min(620, std::max(420, width() - 64)),
                std::min(460, std::max(320, height() - 64)));
  dialog.setStyleSheet(this->styleSheet() +
                       QString(" QDialog { background-color: %1; "
                               "border-radius: 16px; border: 1px solid %2; }")
                           .arg(cssColor(tokens.pageMid),
                                cssColor(tokens.border)));

  QVBoxLayout *layout = new QVBoxLayout(&dialog);
  layout->setContentsMargins(40, 40, 40, 40);
  layout->setSpacing(25);

  QLabel *titleLabel = new QLabel("ro-Assist", &dialog);
  titleLabel->setAlignment(Qt::AlignCenter);
  titleLabel->setStyleSheet(
      QString("font-size: 42px; font-weight: 900; color: %1;")
          .arg(cssColor(tokens.text)));

  QLabel *descLabel = new QLabel(&dialog);
  descLabel->setWordWrap(true);
  descLabel->setAlignment(Qt::AlignCenter);
  QString desc =
      RoAssist::UiTexts::aboutDescription(currentLanguageCode());
  descLabel->setText(desc);
  descLabel->setStyleSheet(
      "font-size: 16px; color: " + cssColor(tokens.muted) + ";");

  QLabel *infoLabel = new QLabel(&dialog);
  infoLabel->setObjectName("aboutInfoPanel");
  infoLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  infoLabel->setWordWrap(true);
  infoLabel->setText(
      QString("<b>%1</b><br>Ebubekir Bulut<br><br><b>%2</b><br>2026")
          .arg(RoAssist::UiTexts::developerLabel(currentLanguageCode()))
          .arg(RoAssist::UiTexts::yearLabel(currentLanguageCode())));
  infoLabel->setStyleSheet(
      "color: " + cssColor(tokens.text) +
      "; padding: 14px 18px; border-radius: 12px; background: " +
      cssColor(tokens.glassEnd) + "; border: 1px solid " +
      cssColor(tokens.border) + ";");

  auto *okBtn = new AnimatedButton(AnimatedButton::Primary,
                                   currentTheme == Dark, &dialog);
  okBtn->setText(RoAssist::UiTexts::closeLabel(currentLanguageCode()));
  okBtn->setObjectName("actionButton");
  okBtn->setMinimumSize(180, 48);
  okBtn->setCursor(Qt::PointingHandCursor);
  QObject::connect(okBtn, &QPushButton::clicked, &dialog, &QDialog::accept);

  layout->addStretch();
  layout->addWidget(titleLabel);
  layout->addSpacing(10);
  layout->addWidget(descLabel);
  layout->addSpacing(20);
  layout->addWidget(infoLabel);
  layout->addStretch();
  layout->addWidget(okBtn, 0, Qt::AlignHCenter);

  dialog.exec();
}

void MainWindow::dummyAppStoreAction() {
  if (QProcess::startDetached(QStringLiteral("plasma-discover")))
    return;
  if (QProcess::startDetached(QStringLiteral("discover")))
    return;
  QMessageBox::information(
      this,
      RoAssist::UiTexts::storeVersionTitle(currentLanguageCode()),
      RoAssist::UiTexts::storeVersionMessage(currentLanguageCode()));
}

void MainWindow::startLibraryPackageInstall() {
  if (!isNetworkConnected) {
    QMessageBox::warning(
        this, RoAssist::UiTexts::errorTitle(currentLanguageCode()),
        RoAssist::UiTexts::noInternetMessage(currentLanguageCode()));
    return;
  }
  if (isOperationRunning()) {
    QMessageBox::information(
        this, RoAssist::UiTexts::infoTitle(currentLanguageCode()),
        RoAssist::UiTexts::operationRunningMessage(currentLanguageCode()));
    return;
  }

  libraryInstallButton->setEnabled(false);
  libraryProgressBar->setRange(0, 100);
  libraryProgressBar->setValue(0);
  libraryProgressBar->show();
  libraryStatusLabel->setText(
      RoAssist::UiTexts::librariesInstalling(currentLanguageCode()));

  libraryLogConsole->clear();
  appendLog(RoAssist::UiTexts::librariesInstalling(currentLanguageCode()),
            "#0066cc");
  setOperationRunning(LibraryInstall);

  QString action = isLibraryInstalled ? "upgrade" : "install";
  preparePackageProcessEnvironment();
  updateProcess->start("/usr/bin/pkexec", QStringList()
                                     << "/usr/bin/dnf" << action << "-y" << "gamemode"
                                     << "mangohud" << "vulkan-loader"
                                     << "vulkan-tools");
}

void MainWindow::startPrinterSupportInstall() {
  if (!isNetworkConnected) {
    QMessageBox::warning(
        this, RoAssist::UiTexts::errorTitle(currentLanguageCode()),
        RoAssist::UiTexts::noInternetMessage(currentLanguageCode()));
    return;
  }
  if (isOperationRunning()) {
    QMessageBox::information(
        this, RoAssist::UiTexts::infoTitle(currentLanguageCode()),
        RoAssist::UiTexts::operationRunningMessage(currentLanguageCode()));
    return;
  }

  printerSupportInstallButton->setEnabled(false);
  printerSupportLaterButton->setEnabled(false);
  printerSupportDisableButton->setEnabled(false);
  printerSupportProgressBar->setRange(0, 0);
  printerSupportProgressBar->show();
  printerSupportStatusLabel->setText(
      RoAssist::UiTexts::buildBundle(
          currentLanguageCode(), currentTheme == Dark, logConsole->isVisible(),
          libraryLogConsole->isVisible(), isLibraryInstalled)
          .printerSupportInstalling);
  printerLogConsole->clear();
  printerLogConsole->show();
  appendPrinterLog(printerSupportStatusLabel->text(), "#0066cc");
  setOperationRunning(PrinterSupportInstall);
  printerSupportProcess->start("/usr/bin/pkexec",
                               QStringList() << "/usr/bin/dnf" << "install" << "-y"
                                             << "ro-printer-support");
}

void MainWindow::choosePrinterSupportLater() {
  QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                     QStringLiteral("Project-Ro-ASD"),
                     QStringLiteral("ro-assist"));
  settings.setValue(QStringLiteral("printer/supportPreference"),
                    QStringLiteral("later"));
  settings.sync();
  updateUiTextAndImages();
}

void MainWindow::disablePrinterSupport() {
  QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                     QStringLiteral("Project-Ro-ASD"),
                     QStringLiteral("ro-assist"));
  settings.setValue(QStringLiteral("printer/supportPreference"),
                    QStringLiteral("disabled"));
  settings.sync();
  updateUiTextAndImages();
}

void MainWindow::openPrinterSettings() {
  if (!QProcess::startDetached("kcmshell6",
                               QStringList() << "kcm_printer_manager")) {
    QMessageBox::warning(
        this, RoAssist::UiTexts::errorTitle(currentLanguageCode()),
        RoAssist::UiTexts::componentFailedToStart(currentLanguageCode()));
  }
}

void MainWindow::openScannerApplication() {
  if (!QProcess::startDetached("skanpage")) {
    QMessageBox::warning(
        this, RoAssist::UiTexts::errorTitle(currentLanguageCode()),
        RoAssist::UiTexts::componentFailedToStart(currentLanguageCode()));
  }
}

void MainWindow::openRoControl() {
  if (!RoAssist::RoControlIntegration::open()) {
    QMessageBox::information(
        this, RoAssist::UiTexts::infoTitle(currentLanguageCode()),
        RoAssist::UiTexts::roControlMissingMessage(currentLanguageCode()));
  }
}

void MainWindow::preparePackageProcessEnvironment() {
  QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
  environment.insert(QStringLiteral("LANG"), QStringLiteral("C"));
  environment.insert(QStringLiteral("LC_ALL"), QStringLiteral("C"));
  updateProcess->setProcessEnvironment(environment);
}

void MainWindow::startNextSystemUpdateStep() {
  if (currentSystemUpdateStep >= systemUpdateCommands.size()) {
    finishSystemUpdateWorkflow();
    return;
  }

  const int displayStep = currentSystemUpdateStep + 1;
  const RoAssist::ProcessCommand command =
      systemUpdateCommands.at(currentSystemUpdateStep);
  statusLabel->setText(RoAssist::UiTexts::updateStepStarting(
      currentLanguageCode(), command.label, displayStep,
      systemUpdateCommands.size()));
  progressBar->setRange(0, 100);
  progressBar->setValue(0);
  appendLog(statusLabel->text(), "#0066cc");
  appendLog(RoAssist::SystemUpdateService::commandPreview(command), "#666666");

  ++currentSystemUpdateStep;
  preparePackageProcessEnvironment();
  updateProcess->start(command.program, command.arguments);
}

void MainWindow::finishSystemUpdateWorkflow() {
  updateButton->setEnabled(true);
  progressBar->setRange(0, 100);
  progressBar->setValue(100);

  lastRiskSnapshot = RoAssist::SystemRiskService::collect();
  if (systemUpdateHadFailures) {
    statusLabel->setText(
        RoAssist::UiTexts::updatePartialFailure(currentLanguageCode()));
    appendLog(statusLabel->text(), "#ff4444");
  } else if (lastRiskSnapshot.rebootRequired) {
    statusLabel->setText(
        RoAssist::UiTexts::updateNeedsReboot(currentLanguageCode()));
    appendLog(statusLabel->text(), "#cc7700");
  } else {
    statusLabel->setText(
        RoAssist::UiTexts::processCompleted(currentLanguageCode()));
    appendLog(RoAssist::UiTexts::allOperationsCompleted(
                  currentLanguageCode()),
              "#00cc00");
  }

  systemUpdateCommands.clear();
  currentSystemUpdateStep = 0;
  refreshMaintenanceStatus();
  clearActiveOperation();
}

void MainWindow::handleSystemUpdateStepFinished(
    int exitCode, QProcess::ExitStatus exitStatus) {
  if (isTerminatingIntentionally) {
    statusLabel->setText(RoAssist::UiTexts::updateAborted(currentLanguageCode()));
    progressBar->hide();
    updateButton->setEnabled(true);
    systemUpdateCommands.clear();
    currentSystemUpdateStep = 0;
    clearActiveOperation();
    return;
  }

  if (exitStatus != QProcess::NormalExit || exitCode != 0) {
    systemUpdateHadFailures = true;
    appendLog(RoAssist::UiTexts::processFailedDetails(currentLanguageCode(),
                                                      false),
              "#ff4444");
  }

  startNextSystemUpdateStep();
}

void MainWindow::startUpdate() {
  if (!isNetworkConnected) {
    QMessageBox::warning(
        this, RoAssist::UiTexts::errorTitle(currentLanguageCode()),
        RoAssist::UiTexts::noInternetMessage(currentLanguageCode()));
    return;
  }
  if (isOperationRunning()) {
    QMessageBox::information(
        this, RoAssist::UiTexts::infoTitle(currentLanguageCode()),
        RoAssist::UiTexts::operationRunningMessage(currentLanguageCode()));
    return;
  }

  refreshMaintenanceStatus();
  if (RoAssist::SystemRiskService::isHighRiskGraphicsState(lastRiskSnapshot)) {
    const QMessageBox::StandardButton choice = QMessageBox::warning(
        this, RoAssist::UiTexts::infoTitle(currentLanguageCode()),
        RoAssist::UiTexts::nouveauRiskWarning(
            currentLanguageCode(), lastRiskSnapshot.roControlAvailable),
        QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
    if (choice != QMessageBox::Ok)
      return;
  }

  const bool flatpakAvailable =
      RoAssist::SystemUpdateService::commandExists(QStringLiteral("flatpak"));
  const bool snapAvailable =
      RoAssist::SystemUpdateService::commandExists(QStringLiteral("snap"));
  systemUpdateCommands = RoAssist::SystemUpdateService::buildUpdateCommands(
      flatpakAvailable, snapAvailable);
  if (systemUpdateCommands.isEmpty()) {
    statusLabel->setText(
        RoAssist::UiTexts::processFailedDetails(currentLanguageCode(), false));
    return;
  }
  currentSystemUpdateStep = 0;
  systemUpdateHadFailures = false;

  updateButton->setEnabled(false);
  progressBar->setRange(0, 100);
  progressBar->setValue(0);
  progressBar->show();

  statusLabel->setText(
      RoAssist::UiTexts::updateStarting(currentLanguageCode()));
  logConsole->clear();
  appendLog(RoAssist::UiTexts::updateStarting(currentLanguageCode()),
            "#0066cc");
  setOperationRunning(SystemUpdate);
  startNextSystemUpdateStep();
}

void MainWindow::appendLog(const QString &text, const QString &color) {
  QString formattedText = QString("<span style='color:%1'>%2</span>")
                              .arg(color, text.toHtmlEscaped());
  if (isLibraryOperationActive()) {
    libraryLogConsole->append(formattedText);
    QScrollBar *sb = libraryLogConsole->verticalScrollBar();
    sb->setValue(sb->maximum());
  } else {
    logConsole->append(formattedText);
    QScrollBar *sb = logConsole->verticalScrollBar();
    sb->setValue(sb->maximum());
  }
}

void MainWindow::appendPrinterLog(const QString &text, const QString &color) {
  const QString formattedText = QString("<span style='color:%1'>%2</span>")
                                    .arg(color, text.toHtmlEscaped());
  printerLogConsole->append(formattedText);
  QScrollBar *scrollBar = printerLogConsole->verticalScrollBar();
  scrollBar->setValue(scrollBar->maximum());
}

void MainWindow::handlePrinterSupportOutput() {
  const QString output =
      QString::fromUtf8(printerSupportProcess->readAllStandardOutput());
  for (const QString &line : output.split('\n', Qt::SkipEmptyParts))
    appendPrinterLog(line, currentTheme == Dark ? "#cccccc" : "#e60909ff");

  if (const auto progress =
          RoAssist::UpdateHelpers::parseTransactionProgress(output)) {
    printerSupportProgressBar->setRange(0, progress->total);
    printerSupportProgressBar->setValue(progress->current);
    printerSupportStatusLabel->setText(RoAssist::UiTexts::installingPackages(
        currentLanguageCode(), progress->current, progress->total));
  } else if (const auto percent =
                 RoAssist::UpdateHelpers::parseDownloadPercent(output)) {
    printerSupportProgressBar->setRange(0, 100);
    printerSupportProgressBar->setValue(*percent);
    printerSupportStatusLabel->setText(
        RoAssist::UiTexts::downloading(currentLanguageCode(), *percent));
  }
}

void MainWindow::handlePrinterSupportErrorOutput() {
  const QString output =
      QString::fromUtf8(printerSupportProcess->readAllStandardError());
  for (const QString &line : output.split('\n', Qt::SkipEmptyParts))
    appendPrinterLog(line, "#cc7700");
}

void MainWindow::handlePrinterSupportFinished(
    int exitCode, QProcess::ExitStatus exitStatus) {
  printerSupportInstallButton->setEnabled(true);
  printerSupportLaterButton->setEnabled(true);
  printerSupportDisableButton->setEnabled(true);
  if (exitStatus == QProcess::NormalExit && exitCode == 0) {
    isPrinterSupportInstalled = true;
    printerSupportProgressBar->setRange(0, 100);
    printerSupportProgressBar->setValue(100);
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QStringLiteral("Project-Ro-ASD"),
                       QStringLiteral("ro-assist"));
    settings.setValue(QStringLiteral("printer/supportPreference"),
                      QStringLiteral("installed"));
    settings.sync();
    clearActiveOperation();
    updateUiTextAndImages();
    appendPrinterLog(printerSupportStatusLabel->text(), "#00cc00");
    return;
  } else {
    printerSupportProgressBar->hide();
    printerSupportStatusLabel->setText(
        RoAssist::UiTexts::buildBundle(
            currentLanguageCode(), currentTheme == Dark, logConsole->isVisible(),
            libraryLogConsole->isVisible(), isLibraryInstalled)
            .printerSupportFailed);
    appendPrinterLog(printerSupportStatusLabel->text(), "#ff4444");
  }
  clearActiveOperation();
}

void MainWindow::handlePrinterSupportProcessError(QProcess::ProcessError error) {
  if (error == QProcess::Crashed || error == QProcess::FailedToStart) {
    printerSupportProgressBar->hide();
    printerSupportInstallButton->setEnabled(true);
    printerSupportLaterButton->setEnabled(true);
    printerSupportDisableButton->setEnabled(true);
    printerSupportStatusLabel->setText(
        RoAssist::UiTexts::criticalErrorPrefix(currentLanguageCode()) +
        (error == QProcess::FailedToStart
             ? RoAssist::UiTexts::componentFailedToStart(currentLanguageCode())
             : RoAssist::UiTexts::componentCrashed(currentLanguageCode())));
    printerLogConsole->show();
    clearActiveOperation();
  }
}

void MainWindow::checkDnfErrors(const QString &output) {
  if (output.contains("Waiting for process", Qt::CaseInsensitive) ||
      output.contains("Another app is currently holding the yum lock",
                      Qt::CaseInsensitive)) {
    QString msg = RoAssist::UiTexts::systemBusy(currentLanguageCode());
    if (isLibraryOperationActive())
      libraryStatusLabel->setText(msg);
    else
      statusLabel->setText(msg);
  }
  if (output.contains("Error: Failed to download metadata",
                      Qt::CaseInsensitive) ||
      output.contains("Could not resolve host", Qt::CaseInsensitive)) {
    QString msg = RoAssist::UiTexts::networkError(currentLanguageCode());
    if (isLibraryOperationActive()) {
      libraryStatusLabel->setText(msg);
      libraryInstallButton->setEnabled(true);
    } else {
      statusLabel->setText(msg);
      updateButton->setEnabled(true);
    }
  }
}

void MainWindow::handleUpdateOutput() {
  QString output = QString::fromUtf8(updateProcess->readAllStandardOutput());
  QStringList lines = output.split('\n', Qt::SkipEmptyParts);
  for (const QString &line : lines) {
    appendLog(line, currentTheme == Dark ? "#cccccc" : "#e60909ff");
  }
  checkDnfErrors(output);

  if (RoAssist::UpdateHelpers::containsNoWorkMarker(output)) {
    transactionPhaseStarted = true;
  }

  if (const auto progress =
          RoAssist::UpdateHelpers::parseTransactionProgress(output)) {
    transactionPhaseStarted = true;
    QString t = RoAssist::UiTexts::installingPackages(
        currentLanguageCode(), progress->current, progress->total);
    if (isLibraryOperationActive()) {
      libraryProgressBar->setRange(0, progress->total);
      libraryProgressBar->setValue(progress->current);
      libraryStatusLabel->setText(t);
    } else {
      progressBar->setRange(0, progress->total);
      progressBar->setValue(progress->current);
      statusLabel->setText(t);
    }
  }

  if (!transactionPhaseStarted) {
    if (const auto percent =
            RoAssist::UpdateHelpers::parseDownloadPercent(output)) {
      QString t = RoAssist::UiTexts::downloading(currentLanguageCode(),
                                                 *percent);
      if (isLibraryOperationActive()) {
        libraryProgressBar->setRange(0, 100);
        libraryProgressBar->setValue(*percent);
        libraryStatusLabel->setText(t);
      } else {
        progressBar->setRange(0, 100);
        progressBar->setValue(*percent);
        statusLabel->setText(t);
      }
    }
  }
}

void MainWindow::handleUpdateErrorOutput() {
  QString errorOutput =
      QString::fromUtf8(updateProcess->readAllStandardError());
  QStringList lines = errorOutput.split('\n', Qt::SkipEmptyParts);
  for (const QString &line : lines) {
    if (!line.contains("[sudo]")) {
      appendLog(line, "#cc7700");
    }
  }
  checkDnfErrors(errorOutput);

  if (errorOutput.contains("standard input:1", Qt::CaseInsensitive) ||
      errorOutput.contains("incorrect password", Qt::CaseInsensitive) ||
      errorOutput.contains("try again", Qt::CaseInsensitive) ||
      errorOutput.contains("not authorized", Qt::CaseInsensitive) ||
      errorOutput.contains("operation was cancelled", Qt::CaseInsensitive) ||
      errorOutput.contains("authentication failed", Qt::CaseInsensitive)) {
    isTerminatingIntentionally = true;
    updateProcess->terminate();
    QString t =
        RoAssist::UiTexts::wrongPasswordShort(currentLanguageCode());
    if (isLibraryOperationActive()) {
      libraryProgressBar->hide();
      libraryProgressBar->setValue(0);
      libraryStatusLabel->setText(t);
      libraryInstallButton->setEnabled(true);
      if (!libraryLogConsole->isVisible())
        toggleLibraryLogs();
    } else {
      progressBar->hide();
      progressBar->setValue(0);
      statusLabel->setText(t);
      updateButton->setEnabled(true);
      if (!logConsole->isVisible())
        toggleUpdateLogs();
    }
    QMessageBox::critical(
        this, RoAssist::UiTexts::errorTitle(currentLanguageCode()),
        RoAssist::UiTexts::wrongPasswordDetail(currentLanguageCode()));
  }
}

void MainWindow::handleCheckUpdateFinished(int exitCode,
                                           QProcess::ExitStatus exitStatus) {
  if (transactionPhaseStarted)
    return;

  updateButton->setEnabled(isNetworkConnected);

  switch (RoAssist::UpdateHelpers::classifyCheckUpdateResult(exitCode,
                                                             exitStatus)) {
  case RoAssist::UpdateHelpers::UpdateCheckStatus::UpdatesAvailable:
    statusLabel->setText(
        RoAssist::UiTexts::updateAvailable(currentLanguageCode()));
    break;
  case RoAssist::UpdateHelpers::UpdateCheckStatus::UpToDate:
    statusLabel->setText(
        RoAssist::UiTexts::systemUpToDate(currentLanguageCode()));
    break;
  case RoAssist::UpdateHelpers::UpdateCheckStatus::Failed:
    statusLabel->setText(
        RoAssist::UiTexts::updateCheckFailed(currentLanguageCode()));
    break;
  }
}

void MainWindow::handleCheckLibFinished(int exitCode,
                                        QProcess::ExitStatus exitStatus) {
  if (exitStatus == QProcess::NormalExit && exitCode == 0) {
    isLibraryInstalled = true;
  } else {
    isLibraryInstalled = false;
  }
  updateUiTextAndImages();
  libraryInstallButton->setEnabled(true);
}

void MainWindow::handleUpdateFinished(int exitCode,
                                      QProcess::ExitStatus exitStatus) {
  if (activeOperation == SystemUpdate) {
    handleSystemUpdateStepFinished(exitCode, exitStatus);
    return;
  }

  bool libraryOperation = isLibraryOperationActive();
  if (libraryOperation) {
    libraryInstallButton->setEnabled(true);
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
      isLibraryInstalled = true;
      libraryProgressBar->setRange(0, 100);
      libraryProgressBar->setValue(100);
      libraryStatusLabel->setText(
          RoAssist::UiTexts::processCompleted(currentLanguageCode()));
      appendLog(RoAssist::UiTexts::librariesInstalled(currentLanguageCode()),
                "#00cc00");
      updateUiTextAndImages();
    } else {
      if (!isTerminatingIntentionally) {
        libraryProgressBar->hide();
        libraryStatusLabel->setText(
            RoAssist::UiTexts::processFailed(currentLanguageCode()));
        appendLog(RoAssist::UiTexts::processFailedDetails(
                      currentLanguageCode(), true),
                  "#ff4444");
        if (!libraryLogConsole->isVisible())
          toggleLibraryLogs();
      }
    }
  } else {
    updateButton->setEnabled(true);
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
      progressBar->setRange(0, 100);
      progressBar->setValue(100);
      statusLabel->setText(
          RoAssist::UiTexts::processCompleted(currentLanguageCode()));
      appendLog(RoAssist::UiTexts::allOperationsCompleted(
                    currentLanguageCode()),
                "#00cc00");
    } else {
      if (!isTerminatingIntentionally) {
        progressBar->hide();
        statusLabel->setText(
            RoAssist::UiTexts::processFailed(currentLanguageCode()));
        appendLog(RoAssist::UiTexts::processFailedDetails(
                      currentLanguageCode(), false),
                  "#ff4444");
        if (!logConsole->isVisible())
          toggleUpdateLogs();
      }
    }
  }
  clearActiveOperation();
}

void MainWindow::handleUpdateProcessError(QProcess::ProcessError error) {
  if (isTerminatingIntentionally)
    return;

  QString errorMsg =
      (error == QProcess::FailedToStart)
          ? RoAssist::UiTexts::componentFailedToStart(currentLanguageCode())
          : RoAssist::UiTexts::componentCrashed(currentLanguageCode());

  QMessageBox::critical(
      this, RoAssist::UiTexts::criticalErrorTitle(currentLanguageCode()),
      errorMsg);

  if (isLibraryOperationActive()) {
    libraryProgressBar->hide();
    libraryInstallButton->setEnabled(true);
    libraryStatusLabel->setText(
        RoAssist::UiTexts::criticalErrorPrefix(currentLanguageCode()) +
        errorMsg);
    if (!libraryLogConsole->isVisible())
      toggleLibraryLogs();
  } else {
    progressBar->hide();
    updateButton->setEnabled(true);
    statusLabel->setText(
        RoAssist::UiTexts::criticalErrorPrefix(currentLanguageCode()) +
        errorMsg);
    if (!logConsole->isVisible())
      toggleUpdateLogs();
  }
  clearActiveOperation();
}
