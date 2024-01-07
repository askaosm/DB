#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include "mysql.h"

#pragma comment(lib, "libmysql.lib")

const char* host = "localhost";
const char* user = "root";
const char* pw = "asuka0820";
const char* db = "package_delivery_system";

void execute(MYSQL* connection, char* queries);
int main(void) {

	MYSQL* connection = NULL;
	MYSQL conn;
	if (mysql_init(&conn) == NULL)
		printf("mysql_init() error!");

	connection = mysql_real_connect(&conn, host, user, pw, db, 3306, (const char*)NULL, 0);
	if (connection == NULL)
	{
		printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
		return 1;
	}

	else
	{
		printf("Connection Succeed\n");

		if (mysql_select_db(&conn, db))
		{
			printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
			return 1;
		}

		int type = -1;
		do {

			printf("----------SELECT QUERY TYPES----------\n\n");

			printf("\t1. TYPE I\n");
			printf("\t2. TYPE II\n");
			printf("\t3. TYPE III\n");
			printf("\t4. TYPE IV\n");
			printf("\t5. TYPE V\n");
			printf("\t0. QUIT\n");
			scanf("%d", &type);


			if (type == 1) {
				int subtype;
				printf("-----subtypes in TYPE I-----\n");
				printf("   1. TYPE I-1.\n");
				printf("   2. TYPE I-2.\n");
				printf("   3. TYPE I-3.\n");
				scanf("%d", &subtype);
				printf("\n");

				printf("Assume truck 1721 is destroyed in a crash\n");
				if (subtype == 1) {
					printf("---- TYPE I-1-----\n");
					printf("Find all customers who had a package on the truck at the time of the crash.\n\n");
					char query[] = "SELECT customer.name "
						"FROM customer "
						"JOIN shipper ON shipper.customer_id = customer.customer_id "
						"JOIN shipper_shipment_company ON shipper.package_id = shipper_shipment_company.package_id "
						"JOIN tracking ON shipper_shipment_company.shipment_id = tracking.shipment_id "
						"JOIN vehicle ON tracking.vehicle_id = vehicle.vehicle_id "
						"WHERE vehicle.vehicle_id = 1721 AND vehicle.status = 'Accident'";
					execute(connection, query);

					printf("\n");

				}
				else if (subtype == 2) {
					printf("---- TYPE I-2-----\n");
					printf("Find all recipients who had a package on that truck at the time of crash\n\n");
					char query[] = "SELECT recipient.name "
						"FROM recipient "
						"JOIN shipper ON shipper.recipient_id = recipient.recipient_id "
						"JOIN shipper_shipment_company ON shipper.package_id = shipper_shipment_company.package_id "
						"JOIN tracking ON shipper_shipment_company.shipment_id = tracking.shipment_id "
						"JOIN vehicle ON tracking.vehicle_id = vehicle.vehicle_id "
						"JOIN shipment_company ON shipper_shipment_company.shipment_id = shipment_company.shipment_id "
						"WHERE vehicle.vehicle_id = 1721 "
						"AND vehicle.status = 'Accident';";
					execute(connection, query);
					printf("\n");

				}
				else if (subtype == 3) {
					printf("---- TYPE I-3-----\n");
					printf("Find the last successful delivery by that truck prior to the crush\n\n");
					char query[] =
						"SELECT shipper.package_id, MAX(shipper.delivery_time) "
						"FROM shipper "
						"JOIN shipper_shipment_company ON shipper.package_id = shipper_shipment_company.package_id "
						"JOIN shipment_company ON shipper_shipment_company.shipment_id = shipment_company.shipment_id "
						"JOIN tracking  ON shipment_company.shipment_id = tracking.shipment_id "
						"JOIN vehicle  ON tracking.vehicle_id = vehicle.vehicle_id "
						"WHERE vehicle.vehicle_id = 1721 AND shipper.delivery_status = 'Delivered' "
						"GROUP BY shipper.package_id "
						"ORDER BY MAX(shipper.delivery_time) ; ";




					execute(connection, query);
					printf("\n");
				}
			}
			else if (type == 2) {

				printf("---- TYPE II-----\n");
				printf("Find the customer who has shipped the most packages in the past year.\n\n");

				int year;
				printf("Which year? :"); scanf("%d", &year);
				char query[300];
				sprintf(query, "SELECT customer.name "
					"FROM customer "
					"JOIN shipper ON customer.customer_id = shipper.customer_id "
					"JOIN ordering ON shipper.package_id = ordering.package_id "
					"WHERE YEAR(ordering.order_time) = %d "
					"GROUP BY customer.customer_id "
					"ORDER BY COUNT(*); ", year);


				printf("customer name: ");
				execute(connection, query);
				printf("\n");


			}
			else if (type == 3) {
				printf("---- TYPE III-----\n");
				printf(" Find the customer who has spent the most money on shipping in the past year\n\n");

				int year;
				printf("Which year? :"); scanf("%d", &year);

				char query[200];
				sprintf(query, "SELECT customer.name "
					"FROM customer "
					"JOIN shipper ON customer.customer_id = shipper.customer_id "
					"JOIN ordering ON shipper.package_id = ordering.package_id "
					"JOIN service ON shipper.service_id = service.service_id "
					"WHERE YEAR(ordering.order_time) = %d "
					"GROUP BY customer.customer_id "
					"ORDER BY SUM(service.price); ", year);

				printf("customer name: ");
				execute(connection, query);

				printf("\n");

			}
			else if (type == 4) {
				printf("---- TYPE IV-----\n");
				printf("Find those packages that were not delivered within the promised time\n\n");
				char query[] = "SELECT shipper.package_id, shipment_company.promise_timestamp, shipper.delivery_time "
					"FROM shipper "
					"JOIN shipper_shipment_company ON shipper.package_id = shipper_shipment_company.package_id "
					"JOIN shipment_company ON shipper_shipment_company.shipment_id = shipment_company.shipment_id "
					"WHERE shipment_company.promise_timestamp < shipper.delivery_time; ";


				execute(connection, query);




				printf("\n");

			}
			else if (type == 5) {
				printf("---- TYPE V-----\n");
				printf("Generate the bill for each customer for the past month. Consider creating several types of bills.\n\n");
				int subtype;
				printf("   1. simple bili.\n");
				printf("   2. A bill listing charges by type of service.\n");
				printf("   3. An itemize billing listing each individual shipment and the charges for it.\n");
				scanf("%d", &subtype);
				printf("\n");
				if (subtype == 1) {

					char query[] = "SELECT customer.name, customer.address, SUM(bill.monthly_amount) AS total_bill "
						"FROM bill "
						"JOIN customer ON bill.customer_id = customer.customer_id "
						"WHERE MONTH(bill.due_data) = MONTH(DATE_SUB(CURDATE(), INTERVAL 1 MONTH)) AND YEAR(bill.due_data) = YEAR(DATE_SUB(CURDATE(), INTERVAL 1 MONTH)) "
						"GROUP BY customer.customer_id; ";
					execute(connection, query);

					printf("\n");

				}
				else if (subtype == 2) {

					char query[] = "SELECT customer.name, service.type_id, SUM(service.price) as total_cost "
						"FROM service "
						"JOIN shipper ON service.service_id = shipper.service_id "
						"JOIN ordering ON shipper.package_id = ordering.package_id "
						"JOIN customer ON ordering.customer_id = customer.customer_id "
						"WHERE MONTH(ordering.order_time) = MONTH(DATE_SUB(CURDATE(), INTERVAL 1 MONTH)) AND YEAR(ordering.order_time) = YEAR(DATE_SUB(CURDATE(), INTERVAL 1 MONTH)) "
						"GROUP BY customer.customer_id, service.type_id; ";

					execute(connection, query);
					printf("\n");

				}
				else if (subtype == 3) {

					char query[] =

						"SELECT customer.name, shipper.package_id, service.price "
						"FROM service "
						"JOIN shipper ON service.service_id = shipper.service_id "
						"JOIN ordering ON shipper.package_id = ordering.package_id "
						"JOIN customer  ON ordering.customer_id = customer.customer_id "
						"WHERE MONTH(ordering.order_time) = MONTH(DATE_SUB(CURDATE(), INTERVAL 1 MONTH)) AND YEAR(ordering.order_time) = YEAR(DATE_SUB(CURDATE(), INTERVAL 1 MONTH)); ";


					execute(connection, query);
					printf("\n");
				}

			}
			else if (type == 0) {
			
				break;
			}
			else {
				printf("Query type number is not correct. ");
			}

		} while (type != 0);


	}
	mysql_close(connection);
	return 0;
}

void execute(MYSQL* connection, char* query) {
	if (mysql_query(connection, query)) {
		fprintf(stderr, "%s\n", mysql_error(connection));
		return;
	}

	MYSQL_RES* sql_result = mysql_store_result(connection);
	if (sql_result == NULL) {
		fprintf(stderr, "%s\n", mysql_error(connection));
		return;
	}

	int num_fields = mysql_num_fields(sql_result);
	MYSQL_ROW sql_row;
	while ((sql_row = mysql_fetch_row(sql_result))) {
		for (int i = 0; i < num_fields; i++) {
			printf("%s \n", sql_row[i]);
		}
		printf("\n");
	}

	mysql_free_result(sql_result);
}


