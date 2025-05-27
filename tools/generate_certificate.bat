@echo off
REM filepath: tools\generate_certificate.bat
REM Script to generate self-signed SSL certificate

echo ======================================================
echo Generating Self-Signed SSL Certificate
echo ======================================================
echo.

REM Store the current directory
set SCRIPT_DIR=%~dp0
echo Certificate files will be saved to: %SCRIPT_DIR%

REM Check if OpenSSL is available in PATH
where openssl >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: OpenSSL not found in PATH.
    echo Please make sure OpenSSL is installed and added to your PATH.
    echo You can download OpenSSL from: https://www.openssl.org/
    echo Default Windows install location: C:\Program Files\OpenSSL-Win64\bin
    echo.
    pause
    exit /b 1
)

echo OpenSSL found. Proceeding with certificate generation...
echo.

REM Generate private key
echo Generating 2048-bit RSA private key...
openssl genrsa -out "%SCRIPT_DIR%key.pem" 2048
if %ERRORLEVEL% neq 0 (
    echo ERROR: Failed to generate private key.
    pause
    exit /b 1
)
echo Private key saved to: %SCRIPT_DIR%key.pem
echo.

REM Generate self-signed certificate
echo Generating self-signed certificate...
echo Please fill in the following information for your certificate:
echo.

openssl req -new -x509 -key "%SCRIPT_DIR%key.pem" -out "%SCRIPT_DIR%cert.pem" -days 365 ^
    -subj "/C=US/ST=State/L=City/O=Organization/OU=Unit/CN=localhost"

if %ERRORLEVEL% neq 0 (
    echo ERROR: Failed to generate certificate.
    pause
    exit /b 1
)
echo Certificate saved to: %SCRIPT_DIR%cert.pem
echo.

REM Verify the certificate
echo Certificate details:
openssl x509 -in "%SCRIPT_DIR%cert.pem" -noout -text | findstr "Subject:"
openssl x509 -in "%SCRIPT_DIR%cert.pem" -noout -text | findstr "Issuer:"
openssl x509 -in "%SCRIPT_DIR%cert.pem" -noout -text | findstr "Not Before"
openssl x509 -in "%SCRIPT_DIR%cert.pem" -noout -text | findstr "Not After"
echo.

echo ======================================================
echo Certificate generation completed successfully!
echo ======================================================
echo.
echo Files generated:
echo - Private Key: %SCRIPT_DIR%key.pem
echo - Certificate: %SCRIPT_DIR%cert.pem
echo.
echo NOTE: This is a self-signed certificate and will not be trusted by browsers.
echo You may see security warnings when using this certificate.
echo.
pause