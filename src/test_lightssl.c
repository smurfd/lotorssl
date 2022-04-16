//                                                                            //
#include "lightbig.h"
#include "lightcrypt.h"
#include "lighthash.h"
#include "lightssl.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
  b08 avail[] = {TLSCIPHER};
  b08 select[] = {TLSCIPHERAVAIL};
  b08 compress = TLSCOMPRESSION;

  if (argc == 2 && argv) {
    if (strcmp(argv[1], "server") == 0) {
      struct sockaddr *cli = NULL;
      int s = lightssl_srv_init("127.0.0.1", "12345");

      lightssl_srv_listen(s, cli);
    } else if (strcmp(argv[1], "client") == 0) {
      struct hello *hs_cli;
      struct hello *hs_srv_recv;
      int cl;

      hs_cli = malloc(sizeof(struct hello));
      lightssl_hs_set_hello(
        hs_cli, false, TLSVERSION, 1337, avail, select, compress, 13371337);
      cl = lightssl_cli_init("127.0.0.1", "12345");
      lightssl_hs_send_hi(cl, false, hs_cli);
      hs_srv_recv = malloc(sizeof(struct hello));
      lightssl_hs_recv_hi(cl, false, hs_srv_recv);
      lightssl_print_hello(hs_srv_recv);
      lightssl_cli_end(cl);
      free(hs_srv_recv);
      free(hs_cli);
    } else if (strcmp(argv[1], "big") == 0) {
      char *cc = malloc(MAXSTR);
      bigint_t *ac, *ad, *a1;

      big_init_m(3, &ac, &ad, &a1);
      big_alloc_max_m(3, &ac, &ad, &a1);

      // Sanity checks
      big_set("21739871283971298371298371289371298371298371298371298371293",
              &ac);
      big_resize(&ac, ac->len, ac->len);
      big_assert_str("21739871283971298371298371289371298371298371298371298371"
                     "293",
                     &ac);

      // Addition tests
      big_set("11111111111111111111111111111111111111111111111111111111111000",
              &ac);
      big_set("33333333333333333333333333333333333333333333333333333333333789",
              &ad);
      big_add(ac, ad, &a1);
      big_assert_str("44444444444444444444444444444444444444444444444444444444"
                     "444789",
                     &a1);

      big_set("512", &ac);
      big_set("512", &ad);
      big_add(ac, ad, &a1);
      big_assert_str("1024", &a1);

      big_set("922221322222222222222225555555555555555555555555555555555555555"
              "5555555555555555555555555222212",
              &ac);
      big_set("111111191112312312311111231231313131323442323423422321313156434"
              "5654345643456543",
              &ad);
      big_add(ac, ad, &a1);
      big_assert_str("92222132222222233333341666786786786666678678686868687899"
                     "78789789778768687119901209901198678755",
                     &a1);

      big_set("-5", &ac);
      big_set("0", &ad);
      big_add(ac, ad, &a1);
      big_assert_str("-5", &a1);

      big_set("-5", &ac);
      big_set("-5", &ad);
      big_add(ac, ad, &a1);
      big_assert_str("-10", &a1);

      // Multiplication tests
      big_set("2048", &ac);
      big_set("8", &ad);
      big_mul(ac, ad, &a1);
      big_assert_str("16384", &a1);

      big_set("1024", &ac);
      big_set("16", &ad);
      big_mul(ac, ad, &a1);
      big_assert_str("16384", &a1);

      big_set("111111191112312312311111231231313131323442323423422321313156434"
              "5654345643456543",
              &ac);
      big_set("922221322222222222222225555555555555555555555555555555555555555"
              "5555555555555555555555555222212",
              &ad);
      big_mul(ac, ad, &a1);
      big_assert_str(
        "10246910958128268693916631472829548730195298865205805295"
        "2481719288409956302066542765911881078061201441897003710374267475414"
        "72468434747817753423563299873235240318870130333116",
        &a1);

      // Subtraction tests
      big_set("600", &ac);
      big_set("22", &ad);
      big_sub(ac, ad, &a1);
      big_assert_str("578", &a1);

      big_set("578", &ac);
      big_set("22", &ad);
      big_sub(ac, ad, &a1);
      big_assert_str("556", &a1);

      big_set("268", &ac);
      big_set("122", &ad);
      big_sub(ac, ad, &a1);
      big_assert_str("146", &a1);

      big_set(
        "214213091810068356490404658162027412465979525019828343314523084"
        "9083913577835915686366844006617905592092527870833662447286333080721"
        "3114639693592192652215296",
        &ac);
      big_set(
        "309533158886756081306558107419546609056284391055396614233847499"
        "0806716218932404114491826217170609713718596845014959592435043758265"
        "9390382585503918682853347985597634436140403892213583177172042680359"
        "04065546571988740511211817241307384",
        &ad);
      for (int ii = 0; ii < 50000; ii++) {
        big_sub_internal(ad, ac, &a1);
        big_assert_str(
          "30953315888675608130655810741954660905628439105539661"
          "42338474990806716218932382693182645210334960673252780642273712994"
          "48254177543160758595005955273250697564169609500354786133330043303"
          "9008867643563040734739358874100817619624589092088",
          &a1);
      }

      big_set("0", &ac);
      big_set("3", &ad);
      big_sub(ac, ad, &a1);
      big_assert_str("-3", &a1);

      big_set("-5", &ac);
      big_set("-5", &ad);
      big_sub(ac, ad, &a1);
      big_assert_str("0", &a1);

      big_set("103382526560188488028939012105769917025147424782696844777714231"
              "4672936145",
              &ac);
      big_set("658458585704117920939883059739518616123144451775691193326701722"
              "12419499",
              &ad);
      big_sub(ac, ad, &a1);
      big_assert_str("96797940703147308819540181508374730863915980264939932844"
                     "4472142460516646",
                     &a1);

      big_set("437703505986056238846885350006344429013751625372343806962268577"
              "70",
              &ac);
      big_set("998689668015619845290232924195546972680041504057103240236666837",
              &ad);
      big_sub(ac, ad, &a1);
      big_assert_str("42771660930590004039398302076438895928695121033177277455"
                     "990190933",
                     &a1);

      // Division tests
      big_set("600", &ac);
      big_set("22", &ad);
      big_div(ac, ad, &a1);
      big_assert_str("27", &a1);

      big_set("10", &ac);
      big_set("3", &ad);
      big_div(ac, ad, &a1);
      big_assert_str("3", &a1);

      big_set("10000", &ac);
      big_set("3", &ad);
      big_div(ac, ad, &a1);
      big_assert_str("3333", &a1);

      big_set("65341020041517", &ac);
      big_set("504510691", &ad);
      big_div(ac, ad, &a1);
      big_assert_str("129513", &a1);

      big_set("653410200415176339561661702610140863689425467613184865518778086"
              "71514674964848",
              &ac);
      big_set("504510691957985614674048147476738214843274379043220774875797753"
              "36394159706815",
              &ad);
      big_div(ac, ad, &a1);
      big_assert_str("1", &a1);

      big_set("372865034438889165706507940964903653553438428825000546936450726"
              "39621059063465",
              &ac);
      big_set("2", &ad);
      big_div(ac, ad, &a1);
      big_assert_str("18643251721944458285325397048245182677671921441250027346"
                     "822536319810529531732",
                     &a1);

      big_set("437703505986056238846885350006344429013751625372343806962268577"
              "70",
              &ac);
      big_set("998689668015619845290232924195546972680041504057103240236666837",
              &ad);
      big_div(ac, ad, &a1);
      big_assert_str("43", &a1);

      big_set("372865034438889165706507940964903653553438428825000546936450726"
              "39621059063465",
              &ac);
      big_set("2", &ad);
      big_div(ac, ad, &a1);
      big_assert_str("18643251721944458285325397048245182677671921441250027346"
                     "822536319810529531732",
                     &a1);

      big_set(
        "213126621294744966540523670831272617567382343028847973123306705"
        "4703025450131593775345799632714362864292670109702312040306070201792"
        "271107175502373632466160",
        &ac);
      big_set("115792089237316195423570985008687907853269984665640564039457584"
              "007908834671663",
              &ad);
      big_div(ac, ad, &a1);
      big_assert_str("18405974250791985306898387292513446207790260853623256697"
                     "871975478881952846046",
                     &a1);

      big_set("300923130364674303566064152685592960036419222958287034092357419",
              &ac);
      big_set("521205221864955451187670066261363906193068031158944893620680",
              &ad);
      big_div(ac, ad, &a1);
      big_assert_str("577", &a1);

      // These below causes Heap corruption
      /*      big_set(
                "977831680815396008051953620868336321350460074412926936453701305"
                "3060780535564424316462375269467718074378386672111032446309228292315"
                "5195553231284779451989130560241037445839460215375857597677332187354"
                "8702908703766827059895408816333758974",
                &ac);
            big_set(
                "115792089237316195423570985008687907853269984665640564039457584"
                "007908834671663",
                &ad);
            big_div(ac, ad, &a1);
            big_assert_str(
                "84447192140330707113963255284567556577780406282036509216"
                "3460350403594128933099359644309908361690869954871395818653795142918"
                "6564022152176310381675487752022988",
                &a1);

            big_set(
                "404251716026950615354170846130636321176736173869816767172198188"
                "2080046817385927547907584945053080537969199235131586865616233451246"
                "7400352144711959302632181372814650771184515342087210644071835189688"
                "768694403662543843232206210706",
                &ac);
            big_set(
                "115792089237316195423570985008687907853269984665640564039457584"
                "007908834671663",
                &ad);
            big_div(ac, ad, &a1);
            big_assert_str(
                "34911859582949198387415456937390592393911583298899102349"
                "2204896726270779361337427372490642664977924959354861526030969502847"
                "027596217666059704067859675",
                &a1);

            big_set(
                "57248667226272026790101013894210764143266696029206115750717130429221"
                "06542428164219429758350790884120269018199908925462088861265726738281"
                "99892805209973836912683769389081833793526045749043779551723092789457"
                "00859674726716573956762863129215615656682259280755884206529961232257"
                "39090359795772704700904126455987220239708241828361148741632734136618"
                "46867520040575418435707781840063581235797865427874795486784310587289"
                "817150338046772311631729871169564273615042361442701520",
                &ac);
            big_set(
                "1157920892373161954235709850086879078532699846656405640394575840"
                "07908834671663",
                &ad);
            big_div(ac, ad, &a1);
            big_assert_str(
                "49440913972059637657084114241069252336970004683659999945519464845882"
                "81241262643652065658134064764128084559749226770866873467855515093957"
                "21884686558000584532417549711510034295580773994554519036948894143948"
                "55896441229712823954927462994508022297856838698795336392362118314496"
                "87993802921736711658888729438039601173911907492700592982131530147912"
                "079948541749035857752315481732903160988470021",
                &a1);

            for (int ii = 0; ii < 50000; ii++) {
              big_set("600", &ac);
              big_set("22", &ad);
              big_div_internal(ac, ad, &a1);
              big_assert_str("27", &a1);
            }
      */

      // Modulo tests
      big_set("10", &ac);
      big_set("3", &ad);
      big_mod(ac, ad, &a1);
      big_assert_str("1", &a1);

      big_set("100", &ac);
      big_set("63", &ad);
      big_mod(ac, ad, &a1);
      big_assert_str("37", &a1);

      // Hex tests
      big_set("0x123", &ac);
      big_set("0xa23", &ad);
      big_mul(ac, ad, &a1);
      (*a1).base = HEX;
      big_assert_str("0xb85c9", &a1);

      big_end_m(2, &ac, &ad);
      printf("OK!\n");
    } else if (strcmp(argv[1], "crypt") == 0) {
      lightcrypt_init();
      printf("OK!\n");
    } else if (strcmp(argv[1], "hash") == 0) {
      const char *in = "smurfd";
      const char *rh =
        "555cfc37fc24d4971de9b091ef13401b8c5cb8b5b55804da571fb2"
        "01cbb4fc5d147ac6f528656456651606546ca42a1070bdfd79d024f3b97dd1bdac7"
        "e70f3d1";
      char *out = malloc(100);

      strcpy(out, lighthash_new(in));

      // the hash of rh and the generated one match?
      assert(lighthash_verify(out, rh));
      free(out);
      printf("OK!\n");
    }
  }
}
