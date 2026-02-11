/* @file: test_bench_rsa.c
 * #desc:
 *
 * #copy:
 *    Copyright (C) 1970 Public Free Software
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program. If not,
 *    see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <time.h>
#include <demoz/c/stdint.h>
#include <demoz/c/string.h>
#include <demoz/lib/rsa.h>
#include <demoz/lib/bn.h>


uint8_t g_buf[4096];
uint8_t g_p[4096];
uint8_t g_q[4096];
uint8_t g_e[4096];
uint8_t g_d[4096];
uint8_t g_n[4096];
uint8_t g_dp[4096];
uint8_t g_dq[4096];
uint8_t g_qinv[4096];
bn8448_t g_a;

void test_rsa2048(void)
{
	clock_t start, end;
	double time;

	printf("rsa2048\n");

	F_SYMBOL(bn8448_str2num)(&g_a, "172750700912115725343284334066664186018439445034085274703122442158011598220597239827693152362885505057033900901433436059079987621839779415988090990315891772971448998745069660204890198469069430865569440478001450209153379052271842485959410330912825872461113783233629789362774546087760991002793655531441085067169", NULL, 0);
	C_SYMBOL(memcpy)(g_p, g_a.n, RSA2048_PRIME_LEN);

	F_SYMBOL(bn8448_str2num)(&g_a, "163214780602680365883124883340505579849551422697400004219275900811215446757813173860882541556783206376075128498061332072892959447884818263758627828755583518474842721842278033076869461492929536518547879872710930946193727503466863847240729180729766733568731693482954191262697613216041039991727318679925739996063", NULL, 0);
	C_SYMBOL(memcpy)(g_q, g_a.n, RSA2048_PRIME_LEN);

	start = clock();
	for (int32_t i = 0; i < 20; i++) {
		F_SYMBOL(rsa2048_genkey)(g_p, g_q, g_e, g_d, g_n);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("genkey time: %.6f (%.2f/s)\n", time / 20, 20 / time);

	start = clock();
	for (int32_t i = 0; i < 20; i++) {
		F_SYMBOL(rsa2048_gencrt)(g_p, g_q, g_d, g_dp, g_dq, g_qinv);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("gencrt time: %.6f (%.2f/s)\n", time / 20, 20 / time);

	C_SYMBOL(memset)(g_buf, 0, sizeof(g_buf));
	C_SYMBOL(memset)(g_buf, 0xff, 255);

	start = clock();
	for (int32_t i = 0; i < 20; i++) {
		F_SYMBOL(rsa2048_encrypt)(g_e, g_n, g_buf);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("enc time: %.6f (%.2f/s)\n", time / 20, 20 / time);

	start = clock();
	for (int32_t i = 0; i < 20; i++) {
		F_SYMBOL(rsa2048_crt_decrypt)(g_p, g_q, g_dp, g_dq, g_qinv, g_buf);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("dec time: %.6f (%.2f/s)\n", time / 20, 20 / time);
}

void test_rsa3072(void)
{
	clock_t start, end;
	double time;

	printf("rsa3072\n");

	F_SYMBOL(bn8448_str2num)(&g_a, "1417768796583844977383379115663030441425064577026674347704767314553086116109430290770501487825902519110696063784913262219769370065213606552569949995056176049650278583432921129368492668809315942257907723838065442791303299098800677440857971746031159401891024992204402003075854054777776649976075315410635581652939117418524808654054337632113539463230482288684872080664774666940661684076533485867048241367997544084704479956523665357072229093683565530788438946191891221", NULL, 0);
	C_SYMBOL(memcpy)(g_p, g_a.n, RSA3072_PRIME_LEN);

	F_SYMBOL(bn8448_str2num)(&g_a, "2251113782143446057619688746954509085523521556038423992078788891834106252515985562298189312205768270654806407331902265168108886924967003376938409257799506812719385756021134946553197069186724433633447972439597972137688761370337546618880468354925686141900190656687573650866938789037980763883853544664065012265705716752765383443722207510770167537155101995461326011966105893657066704834021255101313743089684169697927667650112921203408592023123253881891316696585104019", NULL, 0);
	C_SYMBOL(memcpy)(g_q, g_a.n, RSA3072_PRIME_LEN);

	start = clock();
	for (int32_t i = 0; i < 20; i++) {
		F_SYMBOL(rsa3072_genkey)(g_p, g_q, g_e, g_d, g_n);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("genkey time: %.6f (%.2f/s)\n", time / 20, 20 / time);

	start = clock();
	for (int32_t i = 0; i < 20; i++) {
		F_SYMBOL(rsa3072_gencrt)(g_p, g_q, g_d, g_dp, g_dq, g_qinv);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("gencrt time: %.6f (%.2f/s)\n", time / 20, 20 / time);

	C_SYMBOL(memset)(g_buf, 0, sizeof(g_buf));
	C_SYMBOL(memset)(g_buf, 0xff, 383);

	start = clock();
	for (int32_t i = 0; i < 20; i++) {
		F_SYMBOL(rsa3072_encrypt)(g_e, g_n, g_buf);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("enc time: %.6f (%.2f/s)\n", time / 20, 20 / time);

	start = clock();
	for (int32_t i = 0; i < 20; i++) {
		F_SYMBOL(rsa3072_crt_decrypt)(g_p, g_q, g_dp, g_dq, g_qinv, g_buf);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("dec time: %.6f (%.2f/s)\n", time / 20, 20 / time);
}

void test_rsa4096(void)
{
	clock_t start, end;
	double time;

	printf("rsa4096\n");

	F_SYMBOL(bn8448_str2num)(&g_a, "20034547231668294903498216222555210377113505762239090839166207982604402130221822988621652814084154523865867916900985494570507764350491688400612086911283745928295719468624163837337751635818833284614529462650290438539310844948180850569184663625556041396162237684699531536881088644528312635411854280214778496559027171600482105946323417083658407759057553979044040309419203833236291304485787138621452740752989297985969526378190182357303474212684172100195531646870559484610671203099667423484284932195163332990426944222394667345654236756637137044348035144811722871153895956927762813129208146239448119335785503074657279520949", NULL, 0);
	C_SYMBOL(memcpy)(g_p, g_a.n, RSA4096_PRIME_LEN);

	F_SYMBOL(bn8448_str2num)(&g_a, "26019366171211005773667355406068365192776673261675181136951330387557249729394879590502905887719520352605406248768284665705249345689363511296186879515923020747585486815572179172950120643984576572456082315630459905052403105725100642003124297608009973480048154650016657524657248571226536315067550594714772042687355871719020338315512961340308234701180881866233095852749043437066882784840992686996955652567539865291320910190403156140990986941685846179757714906892003202859835159126540998024167207413753656760041265233410880999826984583848624438591262634756786478288167727334861633913214348767792641114842215720220452930661", NULL, 0);
	C_SYMBOL(memcpy)(g_q, g_a.n, RSA4096_PRIME_LEN);

	start = clock();
	for (int32_t i = 0; i < 20; i++) {
		F_SYMBOL(rsa4096_genkey)(g_p, g_q, g_e, g_d, g_n);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("genkey time: %.6f (%.2f/s)\n", time / 20, 20 / time);

	start = clock();
	for (int32_t i = 0; i < 20; i++) {
		F_SYMBOL(rsa4096_gencrt)(g_p, g_q, g_d, g_dp, g_dq, g_qinv);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("gencrt time: %.6f (%.2f/s)\n", time / 20, 20 / time);

	C_SYMBOL(memset)(g_buf, 0, sizeof(g_buf));
	C_SYMBOL(memset)(g_buf, 0xff, 511);

	start = clock();
	for (int32_t i = 0; i < 20; i++) {
		F_SYMBOL(rsa4096_encrypt)(g_e, g_n, g_buf);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("enc time: %.6f (%.2f/s)\n", time / 20, 20 / time);

	start = clock();
	for (int32_t i = 0; i < 20; i++) {
		F_SYMBOL(rsa4096_crt_decrypt)(g_p, g_q, g_dp, g_dq, g_qinv, g_buf);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("dec time: %.6f (%.2f/s)\n", time / 20, 20 / time);
}

int main(void)
{
	test_rsa2048();
	test_rsa3072();
	test_rsa4096();

	return 0;
}
