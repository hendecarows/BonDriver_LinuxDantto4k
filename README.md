# BonDriver_LinuxDantto4k

[dantto4k][link_dantto4k] を使用して、MMTS (MMT/TLV) から MPEG-2 TS へのリアルタイム変換を行う
Linux 版 BonDriver です。Windows 版 BonDriver_dantto4k.dll を Linux 環境へ移植しました。

## ビルド

### Ubuntu

```bash
sudo apt update
sudo apt install build-essential cmake libssl-dev libpcsclite-dev pcscd pkgconf dvb-tools

git clone https://github.com/hendecarows/BonDriver_LinuxDantto4k.git
cd BonDriver_LinuxDantto4k
git submodule update --init --recursive
```

#### TSDuck のインストール

dantto4k 関係のビルドに [TSDuck][link_tsduck] が必要です。
dantto4k のインストール手順に従い、システムにインストールします。

```bash
cd thirdparty/dantto4k/thirdparty/tsduck
scripts/install-prerequisites.sh
make -j
sudo make install
```

#### BonDriver_LinuxDantto4k のビルド

cmake の手順に従ってビルドします。

```bash
cd BonDriver_LinuxDantto4k
mkdir build
cd build
cmake ..
make -j
```

## インストール

[PT4K][link_pt4k] を 地デジ/BS/BS4K/CS110 の4K対応チューナーとして使用する方法を簡単に示します。

### デバイスの確認

PT4K が [tbs6812_drv][link_tbs6812_drv] により DVB デバイスとして認識されていることを確認します。

```bash
$ dvb-fe-tool -a 0
Device Turbosight TBS 6812 (Dual ISDB-T/S/S3) (/dev/dvb/adapter0/frontend0) capabilities:
$ dvb-fe-tool -a 1
Device Turbosight TBS 6812 (Dual ISDB-T/S/S3) (/dev/dvb/adapter1/frontend0) capabilities:
```

### BonDriver_LinuxDVB の設定

下位層となる DVB デバイスを扱う [BonDriver_LinuxDVB][link_bondriver_linux_dvb] を用意します。
BonDriver_LinuxDVB.ini では、使用するアダプタ番号とチャンネル空間を指定します。

```ini
; BonDriver_LinuxDVB.ini

; adapter0, adapter1 を使用する場合
DvbAdapter=0,1

; チャンネル空間を UHF, BS, BS4K, CS110 の順で定義
; チャンネル空間番号は定義順に0から設定され BS4K=2 です。
Space=UHF,BS,BS4K,CS110
```

EDCB の BonDriver ディレクトリにコピーします。

```bash
sudo cp BonDriver_LinuxDVB.* /usr/local/lib/edcb
```

### BonDriver_LinuxDantto4k の設定

BonDriver_LinuxDantto4k.ini を編集し、MMTS変換を適用するチャンネル空間を指定します。

```ini
; BonDriver_LinuxDantto4k.ini
[bondriver]

; ロードする BonDriver のパスを設定
bondriverPath=/usr/local/lib/edcb/BonDriver_LinuxDVB.so

; MMTS から TS への変換を行うチャンネル空間番号を設定します。
; 上記の例（Space=UHF,BS,BS4K,CS110）では BS4K が 0 開始 の 2 番目のため "2" を指定します。
; これにより、BS4K 以外はパススルー、BS4K のみ TS 変換して出力されます。
mmtsToTsChannelSpace=2

[acas]
; casproxyserverを使用する場合に設定します。
; 使用しない場合はコメントで無効化します。
; casProxyServer=127.0.0.1:24000

; ACAS と BCAS など、複数のカードリーダーが接続されている場合は ACAS が接続されている
; カードリーダー名を設定します。
smartCardReaderName=Alcor Link AK9563 00 00
```

同様にコピーします。

```bash
sudo cp BonDriver_LinuxDantto4k.*
```

### EDCB の設定

[EDCB][link_edcb] の EpgDataCap_Bon コマンドでチャンネルスキャンを実行します。

```bash
EpgDataCap_Bon -d BonDriver_LinuxDantto4k.so -chscan
```

EDCB の WEBUI から BonDriver_LinuxDantto4k.so のチューナー数を 2 に変更します。

```text
http://192.168.x.x:5510/legacy/setting_bon.html
```

BS4K は dantto4k 側で B61 デコード処理を行うため EDCB 側のデコード処理を無効化します。
方法は`/var/local/edcb/BonCtrl.ini` を編集し `[SET]` セクションに `000BFFFF=dummy.so` を追加します。詳細は[こちら][link_edcb_decode]を確認して下さい。

```ini
; /var/local/edcb/BonCtrl.ini
[SET]
...
000BFFFF=dummy.so
```

変更を反映させるために EDCB を再起動します。

``` bash
sudo systemctl restart edcb.service
```

## 謝辞

本プロジェクトは、以下のソフトウェアを基に作成されました。

* [dantto4k][link_dantto4k]
* [BonDriver_LinuxPTX][link_ptx]

## ライセンス

[MIT License][link_mit]

[link_dantto4k]: https://github.com/nekohkr/dantto4k
[link_tsduck]: https://github.com/tsduck/tsduck
[link_pt4k]: https://www.newptx.com/
[link_edcb]: https://github.com/xtne6f/EDCB
[link_tbs6812_drv]: https://github.com/otya128/tbs6812_drv
[link_bondriver_linux_dvb]: https://github.com/hendecarows/BonDriver_LinuxDVB
[link_edcb_decode]: https://github.com/xtne6f/EDCB/blob/302a05135786157efdd9e849fee7545132114b60/Document/Readme_EpgDataCap_Bon.txt#L278-L291
[link_ptx]: https://github.com/nns779/BonDriver_LinuxPTX
[link_mit]: LICENSE
